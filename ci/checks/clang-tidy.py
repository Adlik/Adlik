#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess
import sys
import tempfile


def _get_source_files(compilation_database):
    for compile_command in compilation_database:
        yield os.path.join(compile_command['directory'], compile_command['file'])


def _get_compilation_database(build_args):
    subprocess.check_call(['bazel',
                           'build',
                           '//adlik_serving',
                           *build_args,
                           '--experimental_action_listener=//ci:ci-action-listener'])

    return subprocess.check_output(args=['ci/tools/build-compilation-database.py'], universal_newlines=True)


def main(args):
    has_failure = False
    compilation_database_text = _get_compilation_database(args)

    with open('.clang-tidy') as clang_tidy_file:
        clang_tidy_config = clang_tidy_file.read()

    with tempfile.TemporaryDirectory() as build_path:
        with open(os.path.join(build_path, 'compile_commands.json'), 'w') as f:
            f.write(compilation_database_text)

        for source_file in _get_source_files(json.loads(compilation_database_text)):
            if subprocess.run(['clang-tidy',
                               f'-config={clang_tidy_config}',
                               f'-p={build_path}',
                               source_file]).returncode != 0:
                has_failure = True

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main(sys.argv[1:])
