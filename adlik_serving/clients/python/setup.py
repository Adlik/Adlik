# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""Adlik serving Python API.
"""

import sys

from setuptools import find_packages, setup

DOCLINES = __doc__.split('\n')

# Set when releasing a new version of TensorFlow Serving (e.g. 1.0.0).
_VERSION = '0.0.0'

project_name = 'adlik-serving-api'
# Set when building the pip package
if '--project_name' in sys.argv:
    project_name_idx = sys.argv.index('--project_name')
    project_name = sys.argv[project_name_idx + 1]
    sys.argv.remove('--project_name')
    sys.argv.pop(project_name_idx)

_TF_REQ = ['tensorflow==2.1.0']

# GPU build (note: the only difference is we depend on tensorflow-gpu so
# pip doesn't overwrite it with the CPU build)
if 'adlik-serving-api-gpu' in project_name:
    _TF_REQ = ['tensorflow-gpu==2.1.0']


REQUIRED_PACKAGES = [
    'numpy',
    'requests',
    'grpcio>=1.0<2',
    'protobuf>=3.6.0',
] + _TF_REQ

setup(
    name=project_name,
    version=_VERSION.replace('-', ''),
    author='ZTE',
    author_email='ai@zte.com.cn',
    packages=find_packages(),
    description=DOCLINES[0],
    long_description='\n'.join(DOCLINES[2:]),
    license='Apache 2.0',
    keywords='Adlik serving API libraries',
    install_requires=REQUIRED_PACKAGES,
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: Education',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Topic :: Scientific/Engineering',
        'Topic :: Scientific/Engineering :: Mathematics',
        'Topic :: Scientific/Engineering :: Artificial Intelligence',
        'Topic :: Software Development',
        'Topic :: Software Development :: Libraries',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ],
)
