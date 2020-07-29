# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess  # nosec
from typing import NamedTuple, Sequence, Optional
from .protos.generated.model_config_pb2 import ModelInput, ModelOutput


class OptParams(NamedTuple):
    source_path: str
    target_path: str
    max_batch_size: int
    inputs: Optional[Sequence[ModelInput]]
    outputs: Optional[Sequence[ModelOutput]]


def get_version():
    version_txt = os.path.join(_acquire_optimizer_dir(), 'deployment_tools/model_optimizer/version.txt')
    with open(version_txt) as file:
        version = file.readline().replace('\n', '')
    return version


def convert(params: OptParams, optimizer_script_name='mo.py'):
    optimizer_path = os.path.join(_acquire_optimizer_dir(),
                                  'deployment_tools/model_optimizer/{}'.format(optimizer_script_name))
    popen_args = ['python3', optimizer_path,
                  '--input_model', params.source_path,
                  '--model_name', 'model',
                  '--output_dir', params.target_path,
                  '--batch', str(params.max_batch_size),
                  '--log_level', 'WARNING']
    if params.inputs is not None:
        popen_args.extend(['--input', ','.join(i.name for i in params.inputs)])
    if params.outputs is not None:
        popen_args.extend(['--output', ','.join(o.name for o in params.outputs)])
    subprocess.run(popen_args, timeout=600, check=True, universal_newlines=True)  # nosec


def _acquire_optimizer_dir():
    return os.getenv('INTEL_CVSDK_DIR', '/opt/intel/openvino_2020.4.287')
