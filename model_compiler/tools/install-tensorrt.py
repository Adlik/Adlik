#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import glob
import subprocess
import sys
from os import path
from tempfile import TemporaryDirectory


_TOOLS_DIR = path.dirname(path.abspath(__file__))


def main():
    tensorrt_url = (
        "https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2004/x86_64/"
        "python3-libnvinfer_8.4.3-1+cuda11.6_amd64.deb"
    )

    with TemporaryDirectory() as workspace:
        subprocess.run(args=[sys.executable, path.join(_TOOLS_DIR, 'build-wheel.py'), tensorrt_url, workspace],
                       check=True)

        subprocess.run(args=[sys.executable, '-m', 'pip', 'install', *glob.glob('{}/*.whl'.format(workspace))])


if __name__ == '__main__':
    main()
