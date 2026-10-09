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

# Valid baseline arguments.
# Each test modifies one argument while keeping the others valid.
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
    5: "number_of_compiles_required",
    6: "dongle_cooldown",
}


def run_test(description, args, expected=PARSING_ERROR):
    """Run one parser test and report whether it passed."""

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
        print(f"\nCommand: {' '.join(command)}")
        print(f"{RED}[FAILURE]{RESET} {description}")
        print(f"  Program did not finish within {TIMEOUT} seconds.")
        return

    except OSError as error:
        print(f"\nCould not execute {PROGRAM}: {error}")
        sys.exit(1)

    output = result.stdout.strip()

    # Include stderr when diagnosing unexpected behavior.
    if result.stderr.strip():
        output += ("\n" if output else "") + result.stderr.strip()

    if output == expected:
        passed += 1
        print(f"\nCommand: {' '.join(command)}")
        print(f"{GREEN}[OK]{RESET} {description}")

    else:
        failed += 1
        print(f"\nCommand: {' '.join(command)}")
        print(f"{RED}[FAILURE]{RESET} {description}")
        print(f"  Expected: {expected!r}")
        print(f"  Received: {output!r}")
        print(f"  Exit code: {result.returncode}")


def main():
    """Run all parser tests."""

    global passed, failed

    passed = 0
    failed = 0

    print("========================================")
    print("         CODEXION PARSER TESTS")
    print("========================================")

    # --------------------------------------------------
    # 1. Negative numbers
    # --------------------------------------------------

    print("\n--- Negative numbers ---")

    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = "-1"

        run_test(
            f"{name}: negative value",
            args,
        )

    # --------------------------------------------------
    # 2. Non-integer values
    # --------------------------------------------------

    print("\n--- Non-integer numeric arguments ---")

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
                f"{name}: invalid numeric value {value!r}",
                args,
            )

    # --------------------------------------------------
    # 3. Empty numeric arguments
    # --------------------------------------------------

    print("\n--- Empty numeric arguments ---")

    for index, name in NUMERIC_ARGUMENTS.items():
        args = BASE_ARGS.copy()
        args[index] = ""

        run_test(
            f"{name}: empty value",
            args,
        )

    # --------------------------------------------------
    # 4. Zero coders
    # --------------------------------------------------

    print("\n--- Number of coders ---")

    args = BASE_ARGS.copy()
    args[0] = "0"

    run_test(
        "number_of_coders: zero coders",
        args,
    )

    # --------------------------------------------------
    # 5. Invalid scheduler values
    # --------------------------------------------------

    print("\n--- Invalid scheduler values ---")

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

    # --------------------------------------------------
    # 6. Integer overflow
    # --------------------------------------------------

    print("\n--- Integer range ---")

    # Determine the maximum signed value for the platform's C int.
    int_bits = sizeof(c_int) * 8
    int_max = (1 << (int_bits - 1)) - 1

    print(f"Detected C int range: {-int_max - 1} to {int_max}")

    overflow_values = [
        str(int_max + 1),
        str(int_max + 100),
        "9" * (int_bits + 10),
    ]

    for index, name in NUMERIC_ARGUMENTS.items():
        for value in overflow_values:
            args = BASE_ARGS.copy()
            args[index] = value

            run_test(
                f"{name}: value exceeds INT_MAX ({value[:30]}...)",
                args,
            )

    # --------------------------------------------------
    # 7. Final summary
    # --------------------------------------------------

    total = passed + failed

    print("\n========================================")
    print("                SUMMARY")
    print("========================================")
    print(f"Total tests: {total}")
    print(f"{GREEN}Passed: {passed}{RESET}")
    print(f"{RED}Failed: {failed}{RESET}")

    if failed:
        sys.exit(1)

    print(f"\n{GREEN}All tests passed!{RESET}")


if __name__ == "__main__":
    main()
