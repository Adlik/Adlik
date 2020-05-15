import os
import shutil
import subprocess
from distutils.command.build_py import build_py
from os import path
from tempfile import TemporaryDirectory
from urllib import request

import setuptools

_TYPES_PROTO_URL = 'https://github.com/tensorflow/tensorflow/raw/v2.2.0/tensorflow/core/framework/types.proto'


class _MyBuildPy(build_py):
    def run(self):
        root_path = path.dirname(path.abspath(__file__))

        with TemporaryDirectory() as external_dir:
            types_proto_dir = path.join(external_dir, 'tensorflow/core/framework')

            os.makedirs(types_proto_dir, exist_ok=True)

            with request.urlopen(_TYPES_PROTO_URL) as response, \
                    open(path.join(types_proto_dir, 'types.proto'), 'wb') as proto_file:
                shutil.copyfileobj(response, proto_file)

            os.makedirs(path.join(root_path, 'src/model_compiler/protos/generated'), exist_ok=True)

            subprocess.run(args=['protoc',
                                 '-I', path.join(root_path, 'src/model_compiler/protos/src'),
                                 '-I', external_dir,
                                 '--python_out', path.join(root_path, 'src/model_compiler/protos/generated'),
                                 path.join(root_path, 'src/model_compiler/protos/src/model_config.proto')],
                           check=True)

        super().run()


setuptools.setup(
    name='model_compiler',
    packages=setuptools.find_namespace_packages('src', exclude=['generated']),
    cmdclass={'build_py': _MyBuildPy},
    package_dir={'': 'src'},
    package_data={'model_compiler': ['protos/src/*.proto']},
    install_requires=[
        'keras',
        'tensorflow',
        'tf2onnx@git+https://github.com/onnx/tensorflow-onnx.git@3383ff9bfa50e49bf69863983c965e49b94c073b'
    ],
    entry_points={
        'console_scripts': ['model-compiler=model_compiler.main:main'],
    },
    extras_require={
        'test': [
            'bandit',
            'flake8',
            'mypy',
            'pylint',
            'pytest-cov',
            'pytest-xdist'
        ]
    }
)
