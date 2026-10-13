#!/usr/bin/env python3

import subprocess
import sys
from ctypes import sizeof, c_int


PROGRAM = "./codexion"
TIMEOUT = 2

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

PARSING_ERROR = "Parsing error"

BASE_ARGS = [
    "2",       # number_of_coders
    "1000",    # time_to_burnout
    "100",     # time_to_compile
    "100",     # time_to_debug
    "100",     # time_to_refactor
    "5",       # number_of_compiles_required
    "10",      # dongle_cooldown
    "fifo",    # scheduler
]

NUMERIC_ARGUMENTS = {
    0: "number_of_coders",
    1: "time_to_burnout",
    2: "time_to_compile",
    3: "time_to_debug",
    4: "time_to_refactor",
    5: "number_of_compiles",
    6: "dongle_cooldown",
}

passed = 0
failed = 0


def expected_output(args):
    """Build the expected output for valid arguments."""

    names = [
        "number_of_coders",
        "time_to_burnout",
        "time_to_compile",
        "time_to_debug",
        "time_to_refactor",
        "number_of_compiles",
        "dongle_cooldown",
    ]

    lines = []

    for name, value in zip(names, args[:7]):
        lines.append(f"{name}: {int(value)}")

    scheduler = 1 if args[7] == "fifo" else 2
    lines.append(f"scheduler: {scheduler}")

    return "\n".join(lines)


def run_test(description, args, expected=PARSING_ERROR):
    """Run the program with the given arguments and compare its output."""

    global passed, failed

    command = [PROGRAM] + args

    try:
        result = subprocess.run(
            command,
            capture_output=True,
            text=True,
            timeout=TIMEOUT,
        )

    except subprocess.TimeoutExpired:
        failed += 1
        print(f"{RED}[FAILURE]{RESET} {description}")
        print(f"  Command: {' '.join(command)}")
        print(f"  Reason: program timed out after {TIMEOUT} seconds")
        return

    except OSError as error:
        print(f"Cannot execute {PROGRAM}: {error}", file=sys.stderr)
        sys.exit(1)

    output = result.stdout.strip()

    if result.stderr.strip():
        output += ("\n" if output else "") + result.stderr.strip()

    if output == expected:
        passed += 1
        print(f"{GREEN}[OK]{RESET} {description}")
    else:
        failed += 1
        print(f"{RED}[FAILURE]{RESET} {description}")
        print(f"  Command: {' '.join(command)}")
        print(f"  Expected: {expected}")
        print(f"  Received: {output}")
        print(f"  Exit code: {result.returncode}")


def test_invalid_inputs():
    """Test inputs that should produce a parsing error."""

    print("\n=== INVALID INPUT TESTS ===\n")

    # Negative numbers
    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = "-1"

        run_test(
            f"{name}: negative value",
            args,
        )

    # Non-integer values
    invalid_values = [
        "abc",
        "hello",
        "1.5",
        "1,5",
        "1e3",
        "--1",
        "-",
        "+",
        "1a",
        "a1",
        "++1",
    ]

    for index, name in NUMERIC_ARGUMENTS.items():
        for value in invalid_values:
            args = BASE_ARGS.copy()
            args[index] = value

            run_test(
                f"{name}: invalid value {value!r}",
                args,
            )

    # Empty numeric arguments
    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = ""

        run_test(
            f"{name}: empty value",
            args,
        )

    # Zero coders
    args = BASE_ARGS.copy()
    args[0] = "0"

    run_test(
        "number_of_coders: zero",
        args,
    )

    # Invalid scheduler values
    invalid_schedulers = [
        "invalid",
        "FIFO",
        "Fifo",
        "EDF",
        "Edf",
        "FiFo",
        "",
        "1",
        "2",
        "fifo ",
        " fifo",
        "edf ",
        " edf",
    ]

    for scheduler in invalid_schedulers:
        args = BASE_ARGS.copy()
        args[7] = scheduler

        run_test(
            f"scheduler: invalid value {scheduler!r}",
            args,
        )

    # Integer overflow
    int_bits = sizeof(c_int) * 8
    int_max = (1 << (int_bits - 1)) - 1

    overflow_values = [
        str(int_max + 1),
        str(int_max + 100),
        "9" * 100,
    ]

    for index, name in NUMERIC_ARGUMENTS.items():
        for value in overflow_values:
            args = BASE_ARGS.copy()
            args[index] = value

            run_test(
                f"{name}: overflow value",
                args,
            )


def test_valid_inputs():
    """Test inputs that should be accepted and parsed correctly."""

    print("\n=== VALID INPUT TESTS ===\n")

    # Basic valid configurations
    valid_cases = [
        (
            "All numeric arguments equal to one, fifo",
            ["1", "1", "1", "1", "1", "1", "1", "fifo"],
        ),
        (
            "Typical configuration, fifo",
            ["2", "1000", "100", "100", "100", "5", "10", "fifo"],
        ),
        (
            "Typical configuration, edf",
            ["2", "1000", "100", "100", "100", "5", "10", "edf"],
        ),
        (
            "Larger configuration, fifo",
            ["10", "5000", "200", "300", "400", "20", "50", "fifo"],
        ),
        (
            "One coder, edf",
            ["1", "1000", "100", "100", "100", "5", "10", "edf"],
        ),
    ]

    for description, args in valid_cases:
        run_test(
            description,
            args,
            expected_output(args),
        )

    # Zero values for arguments other than number_of_coders
    zero_cases = [
        (1, "time_to_burnout"),
        (2, "time_to_compile"),
        (3, "time_to_debug"),
        (4, "time_to_refactor"),
        (5, "number_of_compiles"),
        (6, "dongle_cooldown"),
    ]

    for index, name in zero_cases:
        args = BASE_ARGS.copy()
        args[index] = "0"

        run_test(
            f"{name}: zero",
            args,
            expected_output(args),
        )

    # All numeric arguments zero except number_of_coders
    args = ["1", "0", "0", "0", "0", "0", "0", "edf"]

    run_test(
        "All numeric arguments zero except number_of_coders",
        args,
        expected_output(args),
    )

    # Verify scheduler mapping
    for scheduler, expected_scheduler in [("fifo", 1), ("edf", 2)]:
        args = BASE_ARGS.copy()
        args[7] = scheduler
        expected = expected_output(args)

        run_test(
            f"Scheduler mapping: {scheduler} -> {expected_scheduler}",
            args,
            expected,
        )

    # Leading plus signs
    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = "+" + args[index]

        run_test(
            f"{name}: leading plus sign",
            args,
            expected_output(args),
        )

    # Leading zeros
    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = "000" + args[index]

        run_test(
            f"{name}: leading zeros",
            args,
            expected_output(args),
        )

    # INT_MAX in each numeric argument
    int_bits = sizeof(c_int) * 8
    int_max = (1 << (int_bits - 1)) - 1

    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = str(int_max)

        run_test(
            f"{name}: INT_MAX",
            args,
            expected_output(args),
        )

    # INT_MAX in all numeric arguments simultaneously
    args = [str(int_max)] * 7 + ["fifo"]

    run_test(
        "All numeric arguments equal to INT_MAX",
        args,
        expected_output(args),
    )


def main():
    """Run all parser tests and print a summary."""

    global passed, failed

    test_invalid_inputs()
    test_valid_inputs()

    total = passed + failed

    print("\n" + "=" * 50)
    print("PARSER TEST SUMMARY")
    print("=" * 50)
    print(f"Total tests: {total}")
    print(f"{GREEN}Passed: {passed}{RESET}")
    print(f"{RED}Failed: {failed}{RESET}")
    print("=" * 50)

    if failed:
        sys.exit(1)

    print(f"\n{GREEN}All tests passed!{RESET}")


if __name__ == "__main__":
    main()
