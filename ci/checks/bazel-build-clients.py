#!/usr/bin/env python3

import os
import re
import subprocess
import sys
import tempfile


def main(args):
    subprocess.check_call(['bazel', 'build', *args, '//adlik_serving/clients/python:build_pip_package'])

    with tempfile.TemporaryDirectory() as wheel_directory:
        subprocess.check_call(['bazel-bin/adlik_serving/clients/python/build_pip_package', wheel_directory])

        wheels = sorted(os.listdir(wheel_directory))

    assert len(wheels) == 2
    assert re.fullmatch(r'adlik_serving_api-\d+\.\d+\.\d+-py2\.py3-none-any\.whl', wheels[0])
    assert re.fullmatch(r'adlik_serving_api_gpu-\d+\.\d+\.\d+-py2\.py3-none-any\.whl', wheels[1])


if __name__ == "__main__":
    main(sys.argv[1:])
