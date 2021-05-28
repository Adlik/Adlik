#!/usr/bin/env python3

# Copyright 2021 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import subprocess


def main():
    subprocess.run(['sh', 'tools/install_tvm.sh'])


if __name__ == '__main__':
    main()
