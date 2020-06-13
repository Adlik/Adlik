#!/usr/bin/env python3

# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Benchmark test.
"""

from setuptools import find_packages, setup

_VERSION = '0.0.0'

_REQUIRED_PACKAGES = [
    'keras==2.2.4',
    'onnx==1.5.0',
    'protobuf==3.6.1',
    'torch==1.3.0',
    'torchvision==0.4.0',
    'requests',
    'tensorflow==1.15.2',
    'jsonschema==3.1.1',
    'networkx==2.3',
    'defusedxml==0.5.0'
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
    keywords='Test serving-lite performance',
    install_requires=_REQUIRED_PACKAGES,
    extras_require={'test': _TEST_REQUIRES}

)
