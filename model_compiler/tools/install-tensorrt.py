#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import glob
import re
import subprocess
import sys
from os import path
from subprocess import Popen
from tempfile import TemporaryDirectory

_PACKAGE = 'python3-libnvinfer_8.4.3-1+cuda11.6_amd64.deb'

_TENSORRT_SOURCES = {
    (3, 5): 'ubuntu1604',
    (3, 6): 'ubuntu1804',
    (3, 8): 'ubuntu2004'
}

_BASE_URL_SOURCES = {
    (3, 6): 'https://developer.download.nvidia.com/compute/cuda/repos/',
    (3, 8): 'https://developer.download.nvidia.com/compute/cuda/repos/'
}

_TOOLS_DIR = path.dirname(path.abspath(__file__))


def _get_cuda_version():
    checker_regex = re.compile(r'\s*libcudart.so.(\d+).(\d+)\s.*', re.DOTALL)

    with Popen(args=['ldconfig', '-p'],
               stdout=subprocess.PIPE,
               universal_newlines=True) as process:
        for line in process.stdout:
            match = checker_regex.fullmatch(line)

            if match:
                return int(match[1]), int(match[2])

    return None


def _get_tensorrt_url():
    python_version = sys.version_info
    source_os = _TENSORRT_SOURCES[(python_version.major, python_version.minor)]
    base_url = _BASE_URL_SOURCES[(python_version.major, python_version.minor)]

    return '{}/{}/x86_64/{}'.format(base_url, source_os, _PACKAGE)


def main():
    tensorrt_url = _get_tensorrt_url()

    with TemporaryDirectory() as workspace:
        subprocess.run(args=[sys.executable, path.join(_TOOLS_DIR, 'build-wheel.py'), tensorrt_url, workspace],
                       check=True)

        subprocess.run(args=[sys.executable, '-m', 'pip', 'install', *glob.glob('{}/*.whl'.format(workspace))])


if __name__ == '__main__':
    main()
