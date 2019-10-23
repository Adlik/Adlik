#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess
import sys


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
    compilation_database = json.loads(_get_compilation_database(args))

    for compile_command in compilation_database:
        return_code = subprocess.run(args=['include-what-you-use', *compile_command['arguments'][1:]],
                                     cwd=compile_command['directory']).returncode

        if return_code != 2:
            has_failure = True

    if has_failure:
        exit(1)


if __name__ == "__main__":
    main(sys.argv[1:])
