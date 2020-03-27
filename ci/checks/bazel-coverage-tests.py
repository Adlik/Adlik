#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import subprocess
import sys


def main(args):
    tests = subprocess.check_output(args=['bazel', 'query', 'tests(//...)'], universal_newlines=True).splitlines()

    subprocess.check_call(['bazel', 'coverage', *tests, *args])


if __name__ == "__main__":
    main(sys.argv[1:])
