#!/usr/bin/env python3

import os
import shutil
import subprocess
import sys


def main(args):
    target_file = '__adlik_serving/adlik_serving'
    subprocess.check_call(['bazel', 'build', *args, '//adlik_serving'])

    os.makedirs(os.path.dirname(target_file), exist_ok=True)

    try:
        os.remove(target_file)
    except FileNotFoundError:
        pass

    shutil.copy2('bazel-bin/adlik_serving/adlik_serving', target_file)


if __name__ == "__main__":
    main(sys.argv[1:])
