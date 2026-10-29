#!/usr/bin/env python3
"""
Runs the P01 Part 1 test suite. This is the same script Gradescope runs.

    python3 run_tests.py              run all tests
    python3 run_tests.py 15           run only test 15
    python3 run_tests.py -v           also show output of passing tests

Every test has an ID (shown in the output). To run one test by itself, for
example in a debugger:  ./vector_tests 15   or   gdb --args ./vector_tests 15

Each test runs in its own process, so a crash (e.g. a double free caught by
AddressSanitizer) only fails that one test.

If this machine's g++ cannot use AddressSanitizer (the Makefile checks), each
test runs under valgrind instead, so memory errors still fail the test.
"""
import shutil
import argparse
import json
import os
import re
import subprocess
import sys

BINARY = './vector_tests'
STUDENT_FILE = 'Vector.cpp'
TIMEOUT_SECONDS = 10
VALGRIND_TIMEOUT_SECONDS = 60
VALGRIND_ERROR_EXIT = 99
# nouserintercepts: let the test harness keep its own operator new/delete
# (it counts allocations); valgrind still checks every access to that memory.
VALGRIND = ['valgrind', '-q', f'--error-exitcode={VALGRIND_ERROR_EXIT}', '--leak-check=no',
            '--soname-synonyms=somalloc=nouserintercepts']
MAX_OUTPUT_LINES = 40

BANNED = [
    (r'#\s*include\s*[<"]\s*(vector|deque|list|forward_list|array|memory|bits/stdc\+\+\.h)\s*[>"]',
     'including a standard container or <memory> header'),
    (r'\bstd\s*::\s*(vector|deque|list|forward_list|array|unique_ptr|shared_ptr)\b',
     'using a standard container or smart pointer'),
]

SAN_ENV = {
    'ASAN_OPTIONS': 'detect_leaks=0:allocator_may_return_null=0',
    'UBSAN_OPTIONS': 'print_stacktrace=1:halt_on_error=1',
}


def strip_comments(src):
    src = re.sub(r'/\*.*?\*/', '', src, flags=re.S)
    return re.sub(r'//[^\n]*', '', src)


def check_banned():
    try:
        with open(STUDENT_FILE) as f:
            src = strip_comments(f.read())
    except FileNotFoundError:
        return f'{STUDENT_FILE} not found.'
    for pattern, why in BANNED:
        m = re.search(pattern, src)
        if m:
            return (f'{STUDENT_FILE} is not allowed to use the standard library containers '
                    f'or smart pointers (found: "{m.group(0)}", {why}).')
    return None


def build():
    proc = subprocess.run(['make', 'vector_tests'], capture_output=True, text=True)
    if proc.returncode != 0:
        return proc.stdout + proc.stderr
    return None


WARNINGS_TEST_NAME = '[Build] Vector.cpp compiles without warnings'
WARNINGS_TEST_POINTS = 2


def check_warnings():
    """Graded test: Vector.cpp must compile without warnings (-Wall -Wextra)."""
    proc = subprocess.run(['make', '-s', 'warnings'], capture_output=True, text=True)
    found = []
    for line in (proc.stdout + proc.stderr).splitlines():
        if re.search(r'Vector\.cpp:\d+:\d+: warning:', line):
            line = line[line.index('Vector.cpp'):]   # drop the directory prefix
            if line not in found:                   # templates repeat each warning
                found.append(line)
    if not found:
        return True, 'No warnings.'
    return False, ('Your code compiles, but with warnings. Fix each one below; '
                   'run `make warnings` to see them in full.\n\n' + '\n'.join(found))


def checker():
    """Returns 'asan', 'valgrind', or None (no memory checking available)."""
    flags = subprocess.run(['make', '-s', 'print-sanitize'], capture_output=True, text=True).stdout
    if 'address' in flags:
        return 'asan'
    if shutil.which('valgrind'):
        return 'valgrind'
    return None


def list_tests():
    out = subprocess.run([BINARY, '--list'], capture_output=True, text=True, check=True).stdout
    tests = []
    for line in out.splitlines()[1:]:   # first line is the header
        test_id, points, name = line.split(None, 2)
        tests.append((int(test_id), name, int(points)))
    return tests


def trim(output):
    lines = output.strip().splitlines()
    if len(lines) > MAX_OUTPUT_LINES:
        lines = lines[:MAX_OUTPUT_LINES] + [f'... ({len(lines) - MAX_OUTPUT_LINES} more lines)']
    return '\n'.join(lines)


def run_test(test_id, mode):
    env = dict(os.environ, **SAN_ENV)
    cmd = [BINARY, str(test_id)]
    timeout = TIMEOUT_SECONDS
    if mode == 'valgrind':
        cmd = VALGRIND + cmd
        timeout = VALGRIND_TIMEOUT_SECONDS
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout, env=env)
    except subprocess.TimeoutExpired:
        return False, f'Timed out after {timeout} seconds (infinite loop?)'
    output = proc.stdout + proc.stderr
    passed = proc.returncode == 0 and proc.stdout.strip().endswith('PASS')
    if not passed and proc.returncode < 0:
        output += f'\nProcess killed by signal {-proc.returncode} (crash).'
    if mode == 'valgrind' and proc.returncode == VALGRIND_ERROR_EXIT:
        # The test's own checks may have passed; the memory error is what failed it.
        stdout = '' if proc.stdout.strip() == 'PASS' else proc.stdout
        output = ('valgrind detected a memory error. Read the first lines below: they say '
                  'what went wrong and which line of Vector.cpp did it.\n\n' + proc.stderr + stdout)
    elif 'AddressSanitizer' in output:
        output = ('AddressSanitizer detected a memory error. Read the first lines below: '
                  'they say what went wrong and which line of Vector.cpp did it.\n\n' + output)
    return passed, trim(output)


def write_json(path, score, max_score, output, tests):
    result = {'score': score, 'output': output, 'tests': tests}
    if max_score is not None:
        result['max_score'] = max_score
    with open(path, 'w') as f:
        json.dump(result, f, indent=2)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('test_id', nargs='?', type=int,
                        help='run only this test (IDs are listed by ./vector_tests --list)')
    parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument('--json', metavar='PATH', help='write Gradescope results.json')
    args = parser.parse_args()

    error = check_banned()
    if error:
        print(error)
        if args.json:
            write_json(args.json, 0, None, error, [])
        sys.exit(1)

    build_errors = build()
    if build_errors:
        msg = 'Compilation failed:\n\n' + build_errors
        print(msg)
        if args.json:
            write_json(args.json, 0, None, trim(msg), [])
        sys.exit(1)

    mode = checker()
    if mode == 'valgrind':
        print('Note: AddressSanitizer is not available on this machine, so each test runs under\n'
              'valgrind instead. Memory errors are still caught, but tests run slower.\n')
    elif mode is None:
        print('WARNING: neither AddressSanitizer nor valgrind is available on this machine.\n'
              'Memory errors (use-after-free, double free, ...) may go unnoticed here but\n'
              'will fail on Gradescope. Test on CSIL or another Linux machine.\n')

    tests = list_tests()
    warnings_id = len(tests) + 1   # the warnings check comes after the C++ tests
    tests.append((warnings_id, WARNINGS_TEST_NAME, WARNINGS_TEST_POINTS))
    if args.test_id is not None:
        tests = [t for t in tests if t[0] == args.test_id]
        if not tests:
            print(f'No test with ID {args.test_id}. IDs are 1 to {warnings_id}; '
                  f'run ./vector_tests --list to see tests 1 to {warnings_id - 1}.')
            sys.exit(1)

    results = []
    score = max_score = 0
    for test_id, name, points in tests:
        if test_id == warnings_id:
            passed, output = check_warnings()
        else:
            passed, output = run_test(test_id, mode)
        earned = points if passed else 0
        score += earned
        max_score += points
        tag = 'PASS' if passed else 'FAIL'
        print(f'{tag} [{earned}/{points}] #{test_id} {name}')
        if not passed or args.verbose:
            for line in output.splitlines():
                print('      ' + line)
        if not passed and test_id == warnings_id:
            print(f'      (check again: make warnings, or python3 run_tests.py {test_id})')
        elif not passed:
            print(f'      (run just this test: ./vector_tests {test_id})')
        results.append({
            'name': f'{test_id}. {name}',
            'score': earned,
            'max_score': points,
            'status': 'passed' if passed else 'failed',
            'output': 'Passed' if passed else output,
            'visibility': 'visible',
        })

    num_passed = sum(r['status'] == 'passed' for r in results)
    summary = f'Passed {num_passed}/{len(results)} tests. Score: {score}/{max_score}'
    print('\n' + summary)
    if args.json:
        write_json(args.json, score, max_score, summary, results)


if __name__ == '__main__':
    main()
