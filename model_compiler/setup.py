#!/usr/bin/env python3

"""
Model compiler.
"""

from setuptools import find_packages, setup

_VERSION = '0.0.0'

_REQUIRED_PACKAGES = [
    'keras==2.2.4',
    'protobuf==3.6.1',
    'onnx==1.5.0',
    'torch==1.1.0',
    'torchvision==0.3.0',
    'requests',
    'tensorflow==1.14.0'
]

_TEST_REQUIRES = [
    'bandit==1.6.0',
    'flake8==3.7.7',
    'pylint==2.3.1',
]

setup(
    name="model_compiler",
    version=_VERSION.replace('-', ''),
    author='ZTE',
    author_email='ai@zte.com.cn',
    packages=find_packages('src'),
    package_dir={'': 'src'},
    description=__doc__,
    license='Apache 2.0',
    keywords='compile serving model',
    install_requires=_REQUIRED_PACKAGES,
    extras_require={'test': _TEST_REQUIRES},
)
