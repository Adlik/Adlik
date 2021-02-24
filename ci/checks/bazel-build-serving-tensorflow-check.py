#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess
import sys


def _echo_var(value):
    print(f'##vso[task.setvariable variable=BuildTensorFlow;isOutput=true]{value}')


def main(args):
    for arg in args:
        print(arg)

    changed_files = subprocess.check_output(args=['git', 'diff', 'HEAD', 'origin/master', '--name-only'],
                                            universal_newlines=True)
    print("All changed files: ", changed_files)

    need_to_build_tf = False
    for file_path in changed_files.splitlines():
        _, file_extension = os.path.splitext(file_path)
        if file_extension in ('.h', '.cc'):
            need_to_build_tf = True
            break

    _echo_var('true') if need_to_build_tf else _echo_var('false')


if __name__ == "__main__":
    main(sys.argv[1:])
