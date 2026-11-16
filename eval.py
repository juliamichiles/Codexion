#!/usr/bin/env python3
"""Automatic checker for the Codexion (42) evaluation sheet.

Usage (from anywhere):
    python3 eval_codexion.py --repo /path/to/codexion
    python3 eval_codexion.py --runs 5 --min-pass 0.8
    python3 eval_codexion.py --lifo-binary ./codexion_lifo     # recode check
    python3 eval_codexion.py --skip-valgrind --skip-tsan

Each sheet item prints OK / FAIL (WARN / INFO / SKIP for things that need a human).
Exit code is 1 if at least one FAIL was printed.
"""
import argparse
import glob
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from collections import Counter

LINE_RE = re.compile(r"^(\d+) (\d+) (has taken a dongle|is compiling|"
					 r"is debugging|is refactoring|burned out)$")
KIND = {"has taken a dongle": "taken", "is compiling": "compiling",
		"is debugging": "debugging", "is refactoring": "refactoring",
		"burned out": "burned"}
CYCLE = ["taken", "taken", "compiling", "debugging", "refactoring"]
BURN_SLACK = 5       # extra ms allowed on top of the 10 ms burnout rule
SMALL_TOL = 3        # ms, for rounding in overlap / cooldown checks
COLORS = {"OK": "32", "FAIL": "31", "WARN": "33", "INFO": "36", "SKIP": "90"}


# ----------------------------------------------------------------- reporting
class Report:
	def __init__(self):
		self.counts = Counter()
		self.color = sys.stdout.isatty()

	def add(self, status, name, detail=""):
		self.counts[status] += 1
		tag = status.ljust(4)
		if self.color:
			tag = f"\033[{COLORS[status]}m{tag}\033[0m"
		print(f"[{tag}] {name}")
		for line in detail.splitlines():
			print("        " + line)
		sys.stdout.flush()

	def section(self, title):
		print(f"\n=== {title} ===")


# ------------------------------------------------------------------- running
def run_prog(cmd, timeout, env=None):
	try:
		p = subprocess.run([str(c) for c in cmd], capture_output=True,
						   timeout=timeout, env=env)
		out, err, rc, to = p.stdout, p.stderr, p.returncode, False
	except subprocess.TimeoutExpired as e:
		out, err, rc, to = e.stdout or b"", e.stderr or b"", None, True
	return {"out": out.decode(errors="replace"),
			"err": err.decode(errors="replace"), "rc": rc, "timeout": to}


def parse(out):
	events, errs = [], []
	if out and not out.endswith("\n"):
		errs.append("output does not end with a newline (cut/interleaved line)")
	for ln in out.splitlines():
		m = LINE_RE.match(ln)
		if not m:
			errs.append(f"malformed or interleaved line: {ln!r}")
			continue
		events.append((int(m[1]), int(m[2]), KIND[m[3]]))
	return events, errs


def params(args, tol):
	n, burn, tc, td, tr, k, cd, sched = args
	return {"n": int(n), "burn": int(burn), "tc": int(tc), "td": int(td),
			"tr": int(tr), "k": int(k), "cd": int(cd), "sched": sched,
			"tol": tol}


def by_coder(ev):
	d = {}
	for t, i, k in ev:
		d.setdefault(i, []).append((t, k))
	return d


# -------------------------------------------------------------------- checks
# every check: (run_result, params) -> list of error strings
def c_format(r, P):
	errs = list(r["perr"])
	if r["timeout"]:
		errs.append("TIMEOUT: program did not stop (deadlock / hang?)")
	if r["rc"] is not None and r["rc"] < 0:
		errs.append(f"CRASH: killed by signal {-r['rc']}")
	last = -1
	for t, i, _ in r["ev"]:
		if t < last:
			errs.append(f"timestamps go backwards at {t}ms")
			break
		last = t
		if not 1 <= i <= P["n"]:
			errs.append(f"coder id {i} out of range")
			break
	return errs


def c_no_burnout(r, P):
	return [f"unexpected burnout: {t} {i} burned out"
			for t, i, k in r["ev"] if k == "burned"]


def c_compiles(r, P):
	cnt = Counter(i for _, i, k in r["ev"] if k == "compiling")
	low = [f"coder {i}: {cnt[i]}/{P['k']} compiles"
		   for i in range(1, P["n"] + 1) if cnt[i] < P["k"]]
	return low


def c_sequence(r, P):
	errs = []
	for i, lst in by_coder(r["ev"]).items():
		idx = 0
		for t, k in lst:
			if k == "burned":
				continue
			if k != CYCLE[idx % 5]:
				errs.append(f"coder {i}: expected '{CYCLE[idx % 5]}' but got "
							f"'{k}' at {t}ms (compile needs exactly 2 "
							f"'taken' first)")
				break
			idx += 1
	return errs


def c_phases(r, P):
	errs, tol = [], P["tol"]
	for i, lst in by_coder(r["ev"]).items():
		for (t0, k0), (t1, k1) in zip(lst, lst[1:]):
			d = t1 - t0
			if (k0, k1) == ("compiling", "debugging"):
				lo, hi, ph = P["tc"], P["tc"] + tol, "compile"
			elif (k0, k1) == ("debugging", "refactoring"):
				lo, hi, ph = P["td"], P["td"] + tol, "debug"
			elif (k0, k1) == ("refactoring", "taken"):
				lo, hi, ph = P["tr"], None, "refactor"
			else:
				continue
			if d < lo - 2 or (hi is not None and d > hi):
				errs.append(f"coder {i}: {ph} lasted {d}ms (expected {lo}"
							f"{'' if hi is None else f'..{hi}'}) at {t0}ms")
	return errs


def c_overlap(r, P):
	"""No dongle duplication: neighbours never compile at the same time."""
	n, tc = P["n"], P["tc"]
	if n < 2:
		return []
	starts = {i: [t for t, k in l if k == "compiling"]
			  for i, l in by_coder(r["ev"]).items()}
	errs = []
	for a in range(1, n + 1):
		b = a % n + 1
		for sa in starts.get(a, []):
			for sb in starts.get(b, []):
				ov = min(sa, sb) + tc - max(sa, sb)
				if ov > SMALL_TOL:
					errs.append(f"coders {a} and {b} (share a dongle) compile "
								f"together: {sa}ms / {sb}ms, overlap {ov}ms")
	return errs


def c_cooldown(r, P):
	"""Two consecutive compiles using the same dongle must be at least
	time_to_compile + cooldown apart (release happens after the compile)."""
	n, tc, cd = P["n"], P["tc"], P["cd"]
	if n < 2:
		return []
	starts = {i: [t for t, k in l if k == "compiling"]
			  for i, l in by_coder(r["ev"]).items()}
	errs = []
	for k in range(n):
		users = {k + 1, (k - 1) % n + 1}
		seq = sorted((s, c) for c in users for s in starts.get(c, []))
		for (s0, c0), (s1, c1) in zip(seq, seq[1:]):
			if s1 < s0 + tc + cd - SMALL_TOL:
				errs.append(f"dongle {k + 1}: coder {c0} compiled at {s0}ms, "
							f"coder {c1} reused it at {s1}ms "
							f"(< {s0 + tc + cd}ms)")
	return errs


def c_burnout(r, P):
	ev = r["ev"]
	b = [e for e in ev if e[2] == "burned"]
	if len(b) != 1:
		return [f"expected exactly one 'burned out' line, got {len(b)}"]
	errs = []
	if ev[-1][2] != "burned":
		errs.append("'burned out' is NOT the last line printed")
	t, i, _ = b[0]
	last = max([x[0] for x in ev if x[1] == i and x[2] == "compiling"],
			   default=0)
	dl = last + P["burn"]
	if t < dl - SMALL_TOL:
		errs.append(f"burnout printed too early: {t}ms (deadline {dl}ms)")
	elif t > dl + 10 + BURN_SLACK:
		errs.append(f"burnout printed {t - dl}ms after the deadline "
					f"({dl}ms), limit is 10ms")
	return errs


def c_single_coder(r, P):
	bad = [e for e in r["ev"] if e[2] == "compiling"]
	return ["single coder must never compile"] if bad else []


# ---------------------------------------------------------------- test cases
class Ctx:
	pass


def run_case(ctx, name, args, checks, scheduler_orders=None):
	P = params(args, ctx.tol)
	checks = [c_format] + checks
	passed, first_err, results = 0, [], []
	for j in range(ctx.runs):
		r = run_prog(["./codexion", *args], ctx.timeout)
		r["ev"], r["perr"] = parse(r["out"])
		results.append(r)
		errs = []
		for chk in checks:
			errs += chk(r, P)
		if errs:
			if not first_err:
				first_err = errs
			os.makedirs(ctx.fails, exist_ok=True)
			tag = re.sub(r"\W+", "_", name)[:40]
			with open(f"{ctx.fails}/{tag}_run{j + 1}.log", "w") as f:
				f.write(f"# ./codexion {' '.join(args)}\n# {errs[:20]}\n")
				f.write(r["out"])
		else:
			passed += 1
	ok = passed / ctx.runs >= ctx.min_pass
	detail = f"./codexion {' '.join(args)}   [{passed}/{ctx.runs} runs clean]"
	if first_err:
		shown = first_err[:4]
		detail += "\nfirst failing run:\n  - " + "\n  - ".join(shown)
		if len(first_err) > 4:
			detail += f"\n  ... +{len(first_err) - 4} more"
		detail += f"\n(logs in {ctx.fails}/)"
	ctx.rep.add("OK" if ok else "FAIL", name, detail)
	return results


def check_make(ctx):
	ctx.rep.section("Preliminaries")
	r = subprocess.run(["make", "re"], capture_output=True, text=True)
	out = r.stdout + r.stderr
	if r.returncode != 0:
		ctx.rep.add("FAIL", "make compiles", out[-600:])
		return False
	warn = [l for l in out.splitlines() if "warning" in l.lower()]
	flags = all(f in out for f in ("-Wall", "-Wextra", "-Werror", "-pthread"))
	ctx.rep.add("OK" if not warn else "FAIL", "make: no warnings",
				"\n".join(warn[:5]))
	ctx.rep.add("OK" if flags else "FAIL",
				"make uses -Wall -Wextra -Werror -pthread")
	ctx.rep.add("OK" if os.path.isfile("codexion") else "FAIL",
				"binary is named 'codexion'")
	r2 = subprocess.run(["make"], capture_output=True, text=True)
	relink = re.search(r"^(cc|gcc|clang) ", r2.stdout, re.M)
	ctx.rep.add("FAIL" if relink else "OK", "make does not relink")
	rules = subprocess.run(["make", "-n", "clean", "fclean", "re", "all"],
						   capture_output=True, text=True)
	ctx.rep.add("OK" if rules.returncode == 0 else "FAIL",
				"Makefile has all/clean/fclean/re")
	return os.path.isfile("codexion")


def check_globals(ctx):
	pat = re.compile(r"^(?!typedef|extern|#|\}|static\s+const|const|struct\s"
					 r"\w+\s*\{|enum)[A-Za-z_][\w\s\*]*\s\**\w+(\[.*\])?"
					 r"(\s*=.*)?;\s*$")
	found = []
	for path in glob.glob("**/*.[ch]", recursive=True):
		for n, line in enumerate(open(path, errors="replace"), 1):
			if "(" not in line and pat.match(line.rstrip("\n")):
				found.append(f"{path}:{n}: {line.strip()}")
	if found:
		ctx.rep.add("WARN", "possible global variables (verify by hand)",
					"\n".join(found[:8]))
	else:
		ctx.rep.add("OK", "no global variable found (heuristic grep)")


def check_readme(ctx):
	ctx.rep.section("README.md")
	if not os.path.isfile("README.md"):
		ctx.rep.add("FAIL", "README.md exists at repo root")
		return
	txt = open("README.md", errors="replace").read()
	lines = txt.splitlines()
	first = lines[0].strip() if lines else ""
	ok = (re.fullmatch(r"[*_]This project has been created as part of the "
					   r"42 curriculum by [\w-]+(, ?[\w-]+)*[*_]", first)
		  and not first.startswith(("**", "__")))
	ctx.rep.add("OK" if ok else "FAIL",
				"first line italicized and correctly formatted", first)
	heads = [re.sub(r"^#+\s*", "", l).lower() for l in lines
			 if l.startswith("#")]
	for need in ("description", "instructions", "resources",
				 "blocking cases handled",
				 "thread synchronization mechanisms"):
		hit = any(need in h for h in heads)
		ctx.rep.add("OK" if hit else "FAIL", f'section "{need}"')
	ai = re.search(r"\bAI\b|\bChatGPT\b|\bClaude\b|\bCopilot\b|\bGemini\b",
				   txt)
	ctx.rep.add("OK" if ai else "FAIL",
				"Resources mentions how AI was used (keyword search)")


def check_args(ctx):
	ctx.rep.section("Argument validation")
	bad = {
		"no arguments": [],
		"7 arguments": "5 800 200 200 200 10 0".split(),
		"9 arguments": "5 800 200 200 200 10 0 fifo 1".split(),
		"negative number": "5 -800 200 200 200 10 0 fifo".split(),
		"non-integer (abc)": "5 abc 200 200 200 10 0 fifo".split(),
		"non-integer (1.5)": "5 800 1.5 200 200 10 0 fifo".split(),
		"empty argument": ["5", "800", "", "200", "200", "10", "0", "fifo"],
		"bad scheduler (lifo)": "5 800 200 200 200 10 0 lifo".split(),
		"bad scheduler (FIFO)": "5 800 200 200 200 10 0 FIFO".split(),
		"overflow": "5 99999999999 200 200 200 10 0 fifo".split(),
	}
	for name, a in bad.items():
		r = run_prog(["./codexion", *a], 5)
		ev, _ = parse(r["out"])
		problems = []
		if r["timeout"]:
			problems.append("hangs")
		elif r["rc"] is None or r["rc"] < 0:
			problems.append("crashes")
		elif r["rc"] == 0:
			problems.append("exit code 0 (should be an error)")
		if ev or r["out"].strip():
			problems.append("printed simulation output on stdout")
		ctx.rep.add("FAIL" if problems else "OK", f"rejects: {name}",
					", ".join(problems))


def easy(ctx):
	ctx.rep.section("Testing - Easy")
	run_case(ctx, "1 coder must burn out at ~800ms",
			 "1 800 200 200 200 10 0 fifo".split(),
			 [c_burnout, c_single_coder, c_sequence])
	base = [c_no_burnout, c_compiles, c_sequence]
	run_case(ctx, "5 coders fifo: no burnout, stops after 10 compiles",
			 "5 2000 200 200 200 10 0 fifo".split(), base)
	run_case(ctx, "5 coders edf: no burnout, stops after 7 compiles",
			 "5 2000 200 200 200 7 0 edf".split(), base)


def less_easy(ctx):
	ctx.rep.section("Testing - Less easy")
	run_case(ctx, "infeasible 5 500: burnout ~500ms, last line, <=10ms",
			 "5 500 200 200 200 10 0 fifo".split(), [c_burnout, c_sequence])
	run_case(ctx, "infeasible 5 500 (edf): same burnout rules",
			 "5 500 200 200 200 10 0 edf".split(), [c_burnout, c_sequence])
	full = [c_no_burnout, c_compiles, c_sequence, c_overlap]
	run_case(ctx, "feasible run: 2x taken before compiling, no interleave, "
			 "no dongle duplication",
			 "5 2000 200 200 200 10 0 fifo".split(), full)
	run_case(ctx, "odd/even edge: 4 coders, no duplication",
			 "4 2000 200 200 200 5 0 edf".split(), full)
	run_case(ctx, "2 coders (both dongles shared), no burnout",
			 "2 2000 200 200 200 5 0 fifo".split(), full)


def medium(ctx):
	ctx.rep.section("Testing - Medium")
	full = [c_no_burnout, c_compiles, c_sequence, c_overlap, c_cooldown,
			c_phases]
	run_case(ctx, "cooldown 400ms respected (fifo)",
			 "5 3000 200 200 200 10 400 fifo".split(), full)
	fifo = run_case(ctx, "cooldown 800ms, heavy contention (fifo)",
					"5 3000 200 200 200 10 800 fifo".split(), full)
	edf = run_case(ctx, "cooldown 800ms, heavy contention (edf)",
				   "5 3000 200 200 200 10 800 edf".split(), full)
	order = lambda r: tuple(i for _, i, k in r["ev"] if k == "compiling")
	fo, eo = {order(r) for r in fifo}, {order(r) for r in edf}
	if fo & eo and len(fo | eo) == 1:
		ctx.rep.add("WARN", "edf vs fifo grant order",
					"identical in every run - difference not observable;\n"
					"explain/demonstrate it manually during the defence")
	else:
		ctx.rep.add("INFO", "edf vs fifo grant order",
					f"{len(fo)} distinct fifo order(s), {len(eo)} edf "
					f"order(s), sets {'differ' if fo != eo else 'equal'}")
	return fifo


def lifo(ctx, fifo_runs):
	ctx.rep.section("Recode - FIFO -> LIFO")
	if not ctx.lifo:
		ctx.rep.add("SKIP", "LIFO recode",
					"do the change live, build it, then re-run with\n"
					"--lifo-binary ./codexion  (reads grant order from logs)")
		return
	args = "5 3000 200 200 200 10 800 fifo".split()
	P = params(args, ctx.tol)
	orders, bad = [], []
	for _ in range(ctx.runs):
		r = run_prog([ctx.lifo, *args], ctx.timeout)
		r["ev"], r["perr"] = parse(r["out"])
		errs = []
		for chk in (c_format, c_no_burnout, c_compiles, c_sequence, c_overlap):
			errs += chk(r, P)
		bad += errs
		orders.append(tuple(i for _, i, k in r["ev"] if k == "compiling"))
	fo = {tuple(i for _, i, k in r["ev"] if k == "compiling") for r in
		  fifo_runs}
	ctx.rep.add("FAIL" if bad else "OK", "LIFO binary: clean run (no burnout, "
				"serialized, no duplication)", "\n".join(bad[:4]))
	ctx.rep.add("OK" if set(orders) != fo else "FAIL",
				"LIFO grant order differs from FIFO order")
	ctx.rep.add("INFO", "LIFO", "also read the code: the change must be in "
				"the heap/queue comparison, not faked in the logs")


# ------------------------------------------------------ memory/thread tools
def tool_summary(err):
	pat = re.compile(r"Possible data race|lock order|Invalid|lost in loss|"
					 r"Conflicting|Uninitialised|Mismatched|ThreadSanitizer")
	lines = err.splitlines()
	for i, l in enumerate(lines):
		if pat.search(l):
			return "\n".join(x for x in lines[i:i + 10] if x.strip())
	return err[-400:]


VG_CASES = ["1 800 200 200 200 10 0 fifo", "5 4000 200 200 200 3 0 fifo",
			"5 4000 200 200 200 3 0 edf", "5 500 200 200 200 10 0 edf"]


def valgrind_checks(ctx):
	ctx.rep.section("Memory leaks / data races (valgrind)")
	if not shutil.which("valgrind"):
		ctx.rep.add("SKIP", "valgrind not installed")
		return
	tools = {
		"memcheck (leaks)": ["--leak-check=full", "--show-leak-kinds=all",
							 "--errors-for-leak-kinds=all"],
		"helgrind": ["--tool=helgrind"],
		"drd": ["--tool=drd"],
	}
	for tname, opts in tools.items():
		for c in VG_CASES:
			r = run_prog(["valgrind", *opts, "--error-exitcode=99", "-q",
						  "./codexion", *c.split()], 300)
			bad = (r["timeout"] or r["rc"] == 99 or
				   re.search(r"ERROR SUMMARY: [1-9]", r["err"]))
			ctx.rep.add("FAIL" if bad else "OK", f"{tname}: {c}",
						tool_summary(r["err"]) if bad else "")


def tsan_check(ctx):
	ctx.rep.section("Data races (-fsanitize=thread)")
	srcs = glob.glob("source/**/*.c", recursive=True) or \
		glob.glob("**/*.c", recursive=True)
	tmp = tempfile.mkdtemp()
	exe = os.path.join(tmp, "codexion_tsan")
	r = subprocess.run(["cc", "-g", "-fsanitize=thread", "-pthread",
						"-I", "include", *srcs, "-o", exe],
					   capture_output=True, text=True)
	if r.returncode != 0:
		ctx.rep.add("SKIP", "could not build TSAN binary", r.stderr[-300:])
		return
	env = dict(os.environ, TSAN_OPTIONS="exitcode=66")
	for c in ["1 800 200 200 200 10 0 fifo", "5 2000 200 200 200 5 0 fifo",
			  "5 2000 200 200 200 5 0 edf", "5 500 200 200 200 10 0 fifo",
			  "5 3000 200 200 200 4 400 edf"]:
		# newer kernels (vm.mmap_rnd_bits=32) break older TSAN runtimes:
		# disabling ASLR for the process fixes "unexpected memory mapping"
		pre = ["setarch", os.uname().machine, "-R"] \
			if shutil.which("setarch") else []
		r = run_prog([*pre, exe, *c.split()], 120, env)
		bad = r["timeout"] or r["rc"] == 66 or "ThreadSanitizer" in r["err"]
		ctx.rep.add("FAIL" if bad else "OK", f"tsan: {c}",
					tool_summary(r["err"]) if bad else "")
	shutil.rmtree(tmp, ignore_errors=True)


# ---------------------------------------------------------------------- main
def main():
	ap = argparse.ArgumentParser()
	ap.add_argument("--repo", default=".")
	ap.add_argument("--runs", type=int, default=3,
					help="repetitions per timing test (default 3)")
	ap.add_argument("--min-pass", type=float, default=0.8,
					help="ratio of clean runs required (default 0.8)")
	ap.add_argument("--timeout", type=int, default=120)
	ap.add_argument("--tol", type=int, default=15,
					help="ms tolerance for phase durations (default 15)")
	ap.add_argument("--lifo-binary")
	ap.add_argument("--fails-dir", default="eval_fails")
	ap.add_argument("--skip-valgrind", action="store_true")
	ap.add_argument("--skip-tsan", action="store_true")
	a = ap.parse_args()

	lifo_path = os.path.abspath(a.lifo_binary) if a.lifo_binary else None
	os.chdir(a.repo)
	ctx = Ctx()
	ctx.rep, ctx.runs, ctx.min_pass = Report(), a.runs, a.min_pass
	ctx.timeout, ctx.tol, ctx.fails = a.timeout, a.tol, a.fails_dir
	ctx.lifo = lifo_path

	if not check_make(ctx):
		ctx.rep.add("FAIL", "no binary: evaluation stops here")
		sys.exit(1)
	check_globals(ctx)
	check_readme(ctx)
	check_args(ctx)
	easy(ctx)
	less_easy(ctx)
	fifo_runs = medium(ctx)
	lifo(ctx, fifo_runs)
	if not a.skip_valgrind:
		valgrind_checks(ctx)
	if not a.skip_tsan:
		tsan_check(ctx)

	c = ctx.rep.counts
	print(f"\nSUMMARY: {c['OK']} OK, {c['FAIL']} FAIL, {c['WARN']} WARN, "
		  f"{c['INFO']} INFO, {c['SKIP']} SKIP")
	sys.exit(1 if c["FAIL"] else 0)


if __name__ == "__main__":
	main()
