#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import subprocess
import sys


def main(args):
    subprocess.check_call([sys.executable, '-m', 'pip', 'install', '-U', 'six'])

    target_file = '__adlik_serving/adlik_serving'
    subprocess.check_call(['bazel', 'build', '//adlik_serving', *args])

    os.makedirs(os.path.dirname(target_file), exist_ok=True)

    try:
        os.remove(target_file)
    except FileNotFoundError:
        pass

    shutil.copy2('bazel-bin/adlik_serving/adlik_serving', target_file)


if __name__ == "__main__":
    main(sys.argv[1:])
