# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Request message definition
"""
import json
import os

import jsonschema

from .log_util import get_logger

_LOGGER = get_logger(__name__)


class _Config:
    def __init__(self, **kwargs):
        self._config = kwargs
        _LOGGER.info('Dump config: %s', self._config)

    def get_attribute(self, name, default=None):
        """
        Get attribute by name
        :param name:
        :param default:
        :return:
        """
        return self._config.get(name, default)


def _getenv(name, default=None, func=None):
    value = os.getenv(name.upper(), default)
    if func is not None:
        value = func(value)
    return value


def _create_config_from_env():
    serving_type = _getenv('serving_type')
    h5_path = _getenv('h5_path')
    script_path = _getenv('script_path')
    export_path = _getenv('export_path')
    onnx_path = _getenv('onnx_path')

    def _env_to_list(name):
        return _getenv(name, '', lambda x: x.split(',') if x else [])

    input_layer_names = _env_to_list('input_layer_names')
    output_layer_names = _env_to_list('output_layer_names')
    input_signatures = _env_to_list('input_signatures')
    output_signatures = _env_to_list('output_signatures')
    input_names = _env_to_list('input_names')
    input_formats = _env_to_list('input_formats')
    output_names = _env_to_list('output_names')

    model_name = _getenv('model_name')
    job_id = _getenv('job_id')
    callback = _getenv('callback')

    max_batch_size = _getenv('max_batch_size', 1, int)
    checkpoint_path = _getenv('checkpoint_path')
    frozen_graph_path = _getenv('frozen_graph_path')
    version = _getenv('version_number', None)

    return _Config(serving_type=serving_type, h5_path=h5_path, script_path=script_path, export_path=export_path,
                   onnx_path=onnx_path, input_layer_names=input_layer_names, output_layer_names=output_layer_names,
                   input_signatures=input_signatures, output_signatures=output_signatures, model_name=model_name,
                   max_batch_size=max_batch_size, job_id=job_id, callback=callback, checkpoint_path=checkpoint_path,
                   frozen_graph_path=frozen_graph_path, input_names=input_names, input_formats=input_formats,
                   output_names=output_names, version=version)


def _create_config_from_file(json_file):
    with open(json_file) as input_file:
        json_message = json.load(input_file)
    return _Config(**json_message)


def create_config(from_source='env', source=None) -> object:
    """
    Create message config, default from environment variables
    :param from_source: from where parse input message, e.g. 'env', 'file', 'object'
    :param source: json message
    :return:
    """
    if from_source not in ['env', 'file', 'object']:
        raise IOError('Create config error, unsupported source {}'.format(from_source))
    if from_source == 'env':
        return _create_config_from_env()
    elif from_source == 'json':
        return _create_config_from_file(source)
    else:
        return create_config_from_obj(**source)


def create_config_from_obj(obj) -> object:
    """
    Create message config from a dictionary which must match config_schema
    :param obj: dict
    :return:
    """
    schema_path = os.path.join(os.path.dirname(__file__), 'config_schema.json')
    with open(schema_path) as schema_file:
        body_schema = json.load(schema_file)

    jsonschema.validate(obj, body_schema)
    return _Config(**obj)
