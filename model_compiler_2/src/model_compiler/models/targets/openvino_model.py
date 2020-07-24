# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess  # nosec
from typing import NamedTuple, Sequence, Tuple
from .. import repository
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


@repository.REPOSITORY.register_target_model('openvino')
class OpenvinoModel(NamedTuple):
    inputs: Sequence[ModelInput]
    outputs: Sequence[ModelOutput]
    origin_model_path: str
    max_batch_size: int = 1

    def get_inputs(self) -> Sequence[ModelInput]:
        return self.inputs

    def get_outputs(self) -> Sequence[ModelOutput]:
        return self.outputs

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)
        _execute_optimize_model(self.origin_model_path, path, self.max_batch_size,
                                self.inputs, self.outputs)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'openvino', _get_version()


def _acquire_sdk_dir():
    return os.getenv('INTEL_CVSDK_DIR', '/opt/intel/openvino_2020.4.287')


def _get_version():
    version_txt = os.path.join(_acquire_sdk_dir(), 'deployment_tools/model_optimizer/version.txt')
    if not os.path.isfile(version_txt):
        return ''
    with open(version_txt) as file:
        version = file.readline().replace('\n', '')
    return version


def _execute_optimize_model(model_path, target_path, max_batch_size, inputs, outputs):
    optimizer_path = os.path.join(_acquire_sdk_dir(), 'deployment_tools/model_optimizer/mo.py')
    if not os.path.exists(optimizer_path):
        raise Exception('mo.py does not exist, path: {}'.format(optimizer_path))
    popen_args = ['python3', optimizer_path]
    popen_args.extend(['--input_model', model_path])
    popen_args.extend(['--model_name', 'model'])
    popen_args.extend(['--output_dir', target_path])
    popen_args.extend(['--batch', str(max_batch_size)])
    popen_args.extend(['--input', ','.join([i.name for i in inputs])])
    popen_args.extend(['--output', ','.join(o.name for o in outputs)])
    popen_args.extend(['--log_level', 'WARNING'])
    subprocess.run(popen_args, timeout=600, check=True, universal_newlines=True,  # nosec
                   stdout=subprocess.PIPE, stderr=subprocess.PIPE)
