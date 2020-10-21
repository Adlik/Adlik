# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess
from os import path

_UBUNTU_VERSION = 'bionic'
_OPENVINO_VERSION = '2020.4.287'

_IMAGES = {
    'model-compiler': {'OPENVINO_VERSION': _OPENVINO_VERSION},
    'serving-openvino': {'OPENVINO_VERSION': _OPENVINO_VERSION},
    'serving-tensorflow-cpu': {}
}

_BASE_DIR = path.dirname(path.abspath(__file__))
_ROOT_DIR = path.dirname(_BASE_DIR)
_DOCKERFILES_DIR = path.join(_BASE_DIR, 'dockerfiles')


def _build_image(name, extra_args=None):
    build_args = {'UBUNTU_VERSION': _UBUNTU_VERSION}

    for key in ['HTTP_PROXY', 'HTTPS_PROXY', 'NO_PROXY', 'http_proxy', 'https_proxy', 'no_proxy']:
        try:
            build_args[key] = os.environ[key]
        except KeyError:
            pass

    if extra_args:
        build_args.update(extra_args)

    args = ['docker', 'build']

    for key, value in sorted(build_args.items()):
        args.extend(['--build-arg', f'{key}={value}'])

    args.extend(['-f', path.join(path.relpath(_DOCKERFILES_DIR, _ROOT_DIR), f'{name}.dockerfile'),
                 '-t', f'adlik/{name}',
                 '.'])

    subprocess.run(args=args, cwd=_ROOT_DIR, check=True)


def main():
    for name, args in sorted(_IMAGES.items()):
        _build_image(name, args)


if __name__ == '__main__':
    main()
