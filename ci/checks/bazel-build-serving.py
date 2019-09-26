#!/usr/bin/env python3

import os
import shutil
import subprocess
import sys


def main(args):
    subprocess.check_call(['bazel', 'build', *args, '//adlik_serving'])

    os.makedirs('__adlik_serving', exist_ok=True)

    shutil.copy2('bazel-bin/adlik_serving/adlik_serving', '__adlik_serving/adlik_serving')


if __name__ == "__main__":
    main(sys.argv[1:])
