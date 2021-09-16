#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import re
import os
import subprocess


def _get_source_files(root_path):
    file_name_regex = re.compile(r'.*\.(cc|cpp|h)')

    for path, dirs, file_names in os.walk(root_path):
        try:
            dirs.remove('.git')
        except ValueError:
            pass

        for file_name in file_names:
            if file_name_regex.fullmatch(file_name):
                yield os.path.join(path, file_name)


def _needs_formatting(file_path):
    with open(file_path, 'rb') as f:
        original_content = f.read()

    formatted_content = subprocess.check_output(args=['clang-format', f'-assume-filename={file_path}'],
                                                input=original_content)

    if formatted_content != original_content:
        print("======")
        print(formatted_content)
        print("======")
    return formatted_content != original_content


def main():
    working_directory = os.getcwd()
    has_failure = False

    for file_path in _get_source_files(working_directory):
        if _needs_formatting(file_path):
            has_failure = True
            print('Formatting needed:', os.path.relpath(file_path, working_directory))

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main()
