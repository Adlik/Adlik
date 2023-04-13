#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Benchmark test.
"""

from setuptools import find_packages, setup

_VERSION = '0.0.0'

_REQUIRED_PACKAGES = [
    'astroid==2.5.3',
    'h5py==3.1.0',
    'onnx==1.13.0',
    'torch==1.13.1',
    'requests',
    'tensorflow==2.10.1',
    'jsonschema==3.2.0',
    'protobuf==3.19.5'
]

_TEST_REQUIRES = [
    'bandit==1.6.0',
    'flake8==3.7.7',
    'pylint==2.3.1',
    'pytest-cov',
    'pytest-xdist'
]

setup(
    name="benchmark",
    version=_VERSION.replace('-', ''),
    author='ZTE',
    author_email='ai@zte.com.cn',
    packages=find_packages('src'),
    package_dir={'': 'src'},
    description=__doc__,
    license='Apache 2.0',
    keywords='Test Adlik performance',
    install_requires=_REQUIRED_PACKAGES,
    extras_require={'test': _TEST_REQUIRES}
)
