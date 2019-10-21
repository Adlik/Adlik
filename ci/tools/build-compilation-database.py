#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import re
import sys


_UNKNOWN_ARGUMENTS = [
    '-fno-canonical-system-headers'
]


def _get_compile_command_files(dump_command_command_action_root):
    compile_command_file_regex = re.compile(r'compile-command-.*\.json')

    for path, dir_names, file_names in os.walk(dump_command_command_action_root):
        if path == dump_command_command_action_root:
            try:
                dir_names.remove('external')
            except ValueError:
                pass

        for file_name in file_names:
            if compile_command_file_regex.fullmatch(file_name):
                yield os.path.join(path, file_name)


def _load_compile_command_file(file_path):
    with open(file_path) as f:
        return json.load(f)


def _sanitize_compile_command(compile_command, execution_root):
    compile_command['directory'] = execution_root

    new_arguments = [arg for arg in compile_command['arguments'] if arg not in _UNKNOWN_ARGUMENTS]

    new_arguments.extend(['-Wno-unknown-warning-option'])

    compile_command['arguments'] = new_arguments

    return compile_command


def main():
    build_root = os.path.dirname(os.readlink('bazel-bin'))
    execution_root = os.path.dirname(os.path.dirname(build_root))

    dump_command_command_action_root = os.path.join(build_root,
                                                    'extra_actions',
                                                    'ci',
                                                    'dump-cpp-compile-command-action')

    compilation_database = []

    for file_path in _get_compile_command_files(dump_command_command_action_root):
        compile_command = _load_compile_command_file(file_path)

        compilation_database.append(_sanitize_compile_command(compile_command, execution_root))

    json.dump(compilation_database, sys.stdout)


if __name__ == "__main__":
    main()
