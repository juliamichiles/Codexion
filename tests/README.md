# Test Scripts

## `eval.py`

Runs the full automated evaluation suite, including build checks, argument validation, simulation behavior, README checks, Valgrind, Helgrind, DRD, and TSan.

```bash
python3 tests/eval.py --repo .
```

Useful options:

```bash
python3 tests/eval.py --repo /path/to/Codexion --runs 5
python3 tests/eval.py --skip-valgrind --skip-tsan
python3 tests/eval.py --lifo-binary ./codexion_lifo
```

## `safe_tight_cases.sh`

Runs predefined tight-contention, cooldown, and short-cycle cases in FIFO and EDF modes. A case is reported as `OK` when no coder burns out.

```bash
./safe_tight_cases.sh
```

## `stress_burnout.sh`

Runs the default Codexion scenario 300 times and saves outputs containing `burned out` under `fails/`.

```bash
chmod +x stress_burnout.sh
./stress_burnout.sh
```

To run a different command, pass it after `--`:

```bash
./stress_burnout.sh -- ./codexion 4 800 200 200 200 5 10 fifo
```
