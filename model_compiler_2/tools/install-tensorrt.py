#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import ctypes
import glob
import subprocess
import sys
from ctypes import CDLL, c_int
from os import path
from tempfile import TemporaryDirectory

_BASE_URL = 'https://developer.download.nvidia.com/compute/machine-learning/repos'
_PACKAGE = 'python3-libnvinfer_7.0.0-1'

_TENSORRT_SOURCES = {
    (3, 5): 'ubuntu1604',
    (3, 6): 'ubuntu1804',
}

_TOOLS_DIR = path.dirname(path.abspath(__file__))


def _get_cuda_version():
    cuda_runtime = CDLL('libcudart.so')
    version = c_int()

    if cuda_runtime.cudaRuntimeGetVersion(ctypes.byref(version)) == 0:
        version_number = version.value

        return version_number // 1000, version_number % 100 // 10

    return None


def _get_tensorrt_url():
    cuda_version_major, cuda_version_minor = _get_cuda_version()
    python_version = sys.version_info
    source_os = _TENSORRT_SOURCES[(python_version.major, python_version.minor)]

    return '{}/{}/x86_64/{}+cuda{}.{}_amd64.deb'.format(_BASE_URL,
                                                        source_os,
                                                        _PACKAGE,
                                                        cuda_version_major,
                                                        cuda_version_minor)


def main():
    tensorrt_url = _get_tensorrt_url()

    with TemporaryDirectory() as workspace:
        subprocess.run(args=[sys.executable, path.join(_TOOLS_DIR, 'build-wheel.py'), tensorrt_url, workspace],
                       check=True)

        subprocess.run(args=[sys.executable, '-m', 'pip', 'install', *glob.glob('{}/*.whl'.format(workspace))])


if __name__ == '__main__':
    main()
