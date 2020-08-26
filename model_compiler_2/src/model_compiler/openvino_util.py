# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
import subprocess  # nosec
import sys
from typing import Dict, List


def get_version():
    version_txt = os.path.join(_acquire_optimizer_base_dir(), 'deployment_tools/model_optimizer/version.txt')

    try:
        file = open(version_txt)
    except FileNotFoundError:
        return 'unknown version'

    with file:
        return file.readline().rstrip()


def execute_optimize_action(params: Dict[str, str]):
    subprocess.run(_args_dict_to_list(params), check=True)  # nosec


def _args_dict_to_list(params: Dict[str, str]) -> List[str]:
    args = [sys.executable, _acquire_optimizer_script_dir(params.pop('script_name'))]
    for key, value in params.items():
        args.extend(['--' + key, value])
    return args


def _acquire_optimizer_script_dir(script_name):
    return os.path.join(_acquire_optimizer_base_dir(), 'deployment_tools/model_optimizer', script_name)


def _acquire_optimizer_base_dir():
    return os.getenv('INTEL_CVSDK_DIR', '/opt/intel/openvino_2020.4.287')
