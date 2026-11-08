#!/usr/bin/env python3

import re
import sys
from pathlib import Path

GREEN = "\033[32m"
RESET = "\033[0m"

def parse_functions(filename):
    source = Path(filename).read_text()

    # Remove comments
    source = re.sub(r'/\*.*?\*/|//.*?$', '', source, flags=re.S | re.M)

    # Match function definitions (not declarations)
    pattern = re.compile(
        r'^[ \t]*'
        r'((?:[\w*]+\s+)+[\w*]+)\s*'
        r'\([^;{}]*\)\s*'
        r'\{',
        re.M
    )

    functions = []

    for match in pattern.finditer(source):
        signature = match.group(0).split('{')[0].strip()
        signature = ' '.join(signature.split())
        functions.append(signature)

    return functions


if __name__ == "__main__":
    print("-> Files with less than 5 functions:")
    for filename in sys.argv[1:]:
        functions = parse_functions(filename)

        if len(functions) != 5:
            print(f"{GREEN}{filename}: {len(functions)} functions{RESET}")

            for function in functions:
                print(function)
            print()

    print("\n-> Files with 5 or more functions:")
    for filename in sys.argv[1:]:
        functions = parse_functions(filename)
        if len(functions) >= 5:
            print(f"{GREEN}{filename}: {len(functions)} functions{RESET}")
            for function in functions:
                print(function)
            print()
