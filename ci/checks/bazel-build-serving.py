#!/usr/bin/env python3

import subprocess
import sys


def main(args):
    subprocess.check_call(['bazel', 'build', *args, '//adlik_serving'])


if __name__ == "__main__":
    main(sys.argv[1:])
