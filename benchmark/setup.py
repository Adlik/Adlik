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
    'h5py<3',
    'keras==2.4.0',
    'onnx==1.7.0',
    'torch==1.4.0',
    'requests',
    'tensorflow==2.4.0',
    'jsonschema==3.2.0'
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
