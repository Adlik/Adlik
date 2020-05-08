#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import argparse
import os
import re

_SCRIPT_REGEX = re.compile(r'(#![^\n]*)?\s*(.*)', re.S)

_SCRIPT_COPYRIGHT_REGEX = re.compile(r"""(#!\S.*

)?# Copyright \d+ ZTE corporation\. All Rights Reserved\.
# SPDX-License-Identifier: Apache-2\.0
(
\S.*)?""", re.S)

_C_COPYRIGHT_REGEX = re.compile(r"""// Copyright \d+ ZTE corporation\. All Rights Reserved\.
// SPDX-License-Identifier: Apache-2\.0
(
\S.*)?""", re.S)

_SCRIPT_COPYRIGHT_TEXT = """# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
"""

_C_COPYRIGHT_TEXT = """// Copyright 2019 ZTE corporation. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0
"""


def _get_arguments():
    parser = argparse.ArgumentParser()

    parser.add_argument('--fix', action='store_true')

    return parser.parse_args()


def _get_source_files(root_path):
    file_name_regex = re.compile(r'.*\.(cc|h|py)')
    for path, dirs, file_names in os.walk(root_path):
        for i in ['.git', 'third_party']:
            if i in dirs:
                dirs.remove(i)

        for file_name in file_names:
            if file_name_regex.fullmatch(file_name):
                yield os.path.join(path, file_name)


def _check_script(file_path):
    with open(file_path) as f:
        return _SCRIPT_COPYRIGHT_REGEX.fullmatch(f.read())


def _check_c_source(file_path):
    with open(file_path) as f:
        return _C_COPYRIGHT_REGEX.fullmatch(f.read())


def _check(file_path):
    if file_path.endswith('.py'):
        return _check_script(file_path)
    elif file_path.endswith('.h') or file_path.endswith('.cc'):
        return _check_c_source(file_path)
    else:
        raise AssertionError


def _fix_script(file_path):
    with open(file_path) as f:
        hash_bang, content = _SCRIPT_REGEX.fullmatch(f.read()).groups()

    with open(file_path, 'w') as f:
        if hash_bang:
            f.write(hash_bang)
            f.write('\n\n')

        f.write(_SCRIPT_COPYRIGHT_TEXT)

        if content:
            f.write('\n')
            f.write(content)


def _fix_c_source(file_path):
    with open(file_path) as f:
        file_content = f.read()

    with open(file_path, 'w') as f:
        f.write(_C_COPYRIGHT_TEXT)

        if file_content:
            f.write('\n')
            f.write(file_content)


def _fix(file_path):
    if file_path.endswith('.py'):
        return _fix_script(file_path)
    elif file_path.endswith('.h') or file_path.endswith('.cc'):
        return _fix_c_source(file_path)
    else:
        raise AssertionError


def main(args):
    working_directory = os.getcwd()
    has_failure = False

    for file_path in _get_source_files(working_directory):
        if not _check(file_path):
            has_failure = True
            print('File does not contain valid copyright notation:', os.path.relpath(file_path, working_directory))

            if args.fix:
                _fix(file_path)

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main(_get_arguments())
