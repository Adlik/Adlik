# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import json
import os
import subprocess  # nosec
from glob import glob
from tempfile import NamedTemporaryFile
from typing import Any, Dict, List, Mapping, NamedTuple, Optional

from ruamel import yaml


class Config(NamedTuple):
    enable_int8: bool = False
    max_batch_size: Optional[int] = None
    quantization_json_path: Optional[str] = None
    checker_yaml_path: Optional[str] = None
    data_source_path: Optional[str] = None
    annotation_file_path: Optional[str] = None
    evaluate: Optional[bool] = True

    @staticmethod
    def from_json(value: Mapping[str, Any]) -> 'Config':
        return Config(enable_int8=value.get('enable_int8', False),
                      max_batch_size=value.get('max_batch_size'),
                      quantization_json_path=value.get('quantization_json_path'),
                      checker_yaml_path=value.get('checker_yaml_path'),
                      data_source_path=value.get('data_source_path'),
                      annotation_file_path=value.get('annotation_file_path'),
                      evaluate=value.get('evaluate', False))

    @staticmethod
    def from_env(env: Mapping[str, str]) -> 'Config':
        return Config(enable_int8=bool(int(env.get('ENABLE_INT8', '0'))),
                      max_batch_size=int(env['MAX_BATCH_SIZE']),
                      quantization_json_path=env.get('QUANTIZATION_JSON_PATH'),
                      checker_yaml_path=env.get('CHECKER_YAML_PATH'),
                      data_source_path=env.get('DATA_SOURCE_PATH'),
                      annotation_file_path=env.get('ANNOTATION_FILE_PATH'),
                      evaluate=bool(int(env.get('EVALUATE', '0'))))


def get_pot_params(output_dir, quantization_json, config):
    params = {'config': quantization_json,
              'output-dir': output_dir,
              'direct-dump': None}
    if config.evaluate:
        params['evaluate'] = None
    return params


def get_temp_checker_yaml(config):
    if config.checker_yaml_path is None:
        return None
    with open(config.checker_yaml_path, 'r', encoding='utf-8') as original_file:
        data = yaml.load(original_file.read(), Loader=yaml.Loader)
        data['models'][0]['name'] = 'model'
        data['models'][0]['datasets'][0]['data_source'] = config.data_source_path
        data['models'][0]['datasets'][0]['annotation_conversion']['annotation_file'] = config.annotation_file_path
        temp_checker_yaml = NamedTemporaryFile(suffix='.yaml')
        with open(temp_checker_yaml.name, 'w', encoding='utf-8') as temp_file:
            yaml.dump(data, temp_file, Dumper=yaml.RoundTripDumper)
    return temp_checker_yaml


def _get_model_file(path, suffix_str):
    return glob(os.path.join(path, suffix_str))[0]


def get_temp_quantization_json(config, model_path, temp_checker_yaml):
    with open(config.quantization_json_path, 'r') as original_file:
        data = json.load(original_file)
        data['model']['model_name'] = 'model'
        data['model']['model'] = _get_model_file(model_path, '*.xml')
        data['model']['weights'] = _get_model_file(model_path, '*.bin')
        if 'type' in data['engine'] and data['engine']['type'] == 'simplified':
            data['engine']['data_source'] = config.data_source_path
        else:
            data['engine']['config'] = temp_checker_yaml.name

        temp_quantization_json = NamedTemporaryFile(suffix='.json')
        with open(temp_quantization_json.name, 'w') as temp_file:
            json.dump(data, temp_file)
    return temp_quantization_json


def execute_pot_action(params: Dict[str, str]):
    subprocess.run(_args_dict_to_list(params), check=True)  # nosec


def _args_dict_to_list(params: Dict[str, str]) -> List[str]:
    args = ['pot']
    for key, value in params.items():
        args.extend(['--' + key] if value is None else ['--' + key, value])
    return args
