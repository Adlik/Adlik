# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import shutil
import subprocess
from os import path
from tempfile import TemporaryDirectory
from urllib import request

import setuptools
from setuptools.command.build_py import build_py


_TYPES_PROTO_URL = 'https://github.com/tensorflow/tensorflow/raw/v2.6.2/tensorflow/core/framework/types.proto'
_PROJECT_PATH = path.dirname(path.abspath(__file__))


def _generate_protos():
    with TemporaryDirectory() as external_dir:
        types_proto_dir = path.join(external_dir, 'tensorflow', 'core', 'framework')
        types_proto_file = path.join(types_proto_dir, 'types.proto')
        protos_dir = path.join(_PROJECT_PATH, 'src', 'model_compiler', 'protos')
        protos_src_dir = path.join(protos_dir, 'src')
        protos_generated_dir = path.join(protos_dir, 'generated')

        os.makedirs(types_proto_dir, exist_ok=True)
        os.makedirs(protos_generated_dir, exist_ok=True)

        with request.urlopen(_TYPES_PROTO_URL) as response, open(types_proto_file, 'wb') as proto_file:
            shutil.copyfileobj(response, proto_file)

        subprocess.run(args=['protoc',
                             '-I', protos_src_dir,
                             '-I', external_dir,
                             '--python_out', protos_generated_dir,
                             path.join(protos_src_dir, 'model_config.proto')],
                       check=True)


class _BuildPy(build_py):
    def run(self):
        _generate_protos()

        super().run()


setuptools.setup(
    name='model_compiler',
    packages=setuptools.find_namespace_packages('src'),
    cmdclass={'build_py': _BuildPy},
    package_dir={'': 'src'},
    package_data={'model_compiler': ['protos/src/*.proto']},
    install_requires=[
        'absl-py==1.3.0',
        'addict==2.4.0',
        'astor==0.8.1',
        'astunparse==1.6.3',
        'cachetools==5.2.0',
        'charset-normalizer==2.1.1',
        'click==8.1.3',
        'coloredlogs==15.0.1',
        'commonmark==0.9.1',
        'Cython==0.29.32',
        'decorator==5.1.1',
        'defusedxml==0.7.1',
        'flatbuffers==2.0.7',
        'future==0.18.2',
        'gast==0.4.0',
        'google-auth==2.12.0',
        'google-auth-oauthlib==0.4.6',
        'google-pasta==0.2.0',
        'grpcio==1.50.0',
        'h5py==3.7.0',
        'humanfriendly==10.0',
        'idna==3.4',
        'importlib-metadata==5.1.0',
        'jstyleson==0.0.2',
        'keras==2.10.0',
        'Keras-Preprocessing==1.1.2',
        'libclang==14.0.6',
        'Markdown==3.4.1',
        'MarkupSafe==2.1.1',
        'mpmath==1.2.1',
        'networkx==2.8',
        'numpy==1.23.1',
        'oauthlib==3.2.2',
        'oneflow==0.7.0+cpu',
        'oneflow-onnx==0.6.1',
        'onnx==1.12.0',
        'onnx-caffe2==1.0.0',
        'onnx-simplifier==0.4.10',
        'onnx-tf==1.9.0',
        'onnxoptimizer==0.3.2',
        'onnxruntime==1.13.1',
        'onnxruntime-gpu==1.13.1',
        'opencv-python==4.6.0.66',
        'openvino==2022.3.0.dev20221103',
        'openvino-dev==2022.3.0.dev20221103',
        'openvino-telemetry==2022.1.1',
        'opt-einsum==3.3.0',
        'paddle-bfloat==0.1.7',
        'paddle2onnx==1.0.0',
        'paddlepaddle==2.3.2',
        'pandas==1.1.5',
        'Pillow==9.3.0',
        'progress==1.6',
        'protobuf==3.19.1',
        'py-cpuinfo==9.0.0',
        'pyasn1==0.4.8',
        'pyasn1-modules==0.2.8',
        'Pygments==2.13.0',
        'python-dateutil==2.8.2',
        'pytz==2022.6',
        'requests==2.28.1',
        'requests-oauthlib==1.3.1',
        'rich==12.6.0',
        'rsa==4.9',
        'ruamel.yaml==0.17.21',
        'ruamel.yaml.clib==0.2.7',
        'scipy==1.9.3',
        'sympy==1.11.1',
        'tensorboard==2.10.1',
        'tensorboard-data-server==0.6.1',
        'tensorboard-plugin-wit==1.8.1',
        'tensorflow==2.10.1',
        'tensorflow-addons==0.18.0',
        'tensorflow-estimator==2.10.0',
        'tensorflow-io-gcs-filesystem==0.28.0',
        'termcolor==2.1.1',
        'test-generator==0.1.1',
        'texttable==1.6.7',
        'tf2onnx==1.13.0',
        'torch==1.8.1',
        'torchvision==0.9.1',
        'tornado==6.1',
        'tqdm==4.64.1',
        'typeguard==2.13.3',
        'urllib3==1.26.12',
        'Werkzeug==2.2.2',
        'zipp==3.10.0'
    ],
    entry_points={'console_scripts': ['model-compiler=model_compiler.main:main']},
    extras_require={
        'test': [
            'bandit==1.7.4',
            'flake8==4.0.1',
            'mypy==0.971',
            'pylint==2.6.2',
            'pytest-cov==3.0.0',
            'pytest-xdist==3.0.2',
            'astroid==2.4.2',
            'attrs==22.1.0',
            'certifi==2022.9.24',
            'coverage==6.5.0',
            'exceptiongroup==1.0.4',
            'execnet==1.9.0',
            'gitdb==4.0.10',
            'GitPython==3.1.29',
            'iniconfig==1.1.1',
            'isort==5.10.1',
            'lazy-object-proxy==1.4.3',
            'mccabe==0.6.1',
            'mypy-extensions==0.4.3',
            'packaging==21.3',
            'pbr==5.11.0',
            'pip==22.2.2',
            'pluggy==1.0.0',
            'pycodestyle==2.8.0',
            'pyflakes==2.4.0',
            'pyparsing==3.0.9',
            'pytest==7.2.0',
            'PyYAML==6.0',
            'setuptools==65.5.0',
            'six==1.16.0',
            'smmap==5.0.0',
            'stevedore==4.1.1',
            'toml==0.10.2',
            'tomli==2.0.1',
            'typing_extensions==4.4.0',
            'wheel==0.37.1',
            'wrapt==1.14.1',
            'types-protobuf==3.20.4.6',
            'types-requests==2.28.11.5',
            'types-setuptools==65.6.0.1',
            'types-urllib3==1.26.25.4'
        ]
    },
    python_requires='== 3.8.15'
)
