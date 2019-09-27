"""
Message from proxy definition
"""
import json
import os

from .log_util import get_logger

_LOGGER = get_logger(__name__)


class _Config:
    def __init__(self, **kwargs):
        if 'job_id' not in kwargs:
            raise Exception('Not found job id in config, config: %s' % kwargs)
        if 'callback' not in kwargs:
            raise Exception('Not found callback in config, config: %s' % kwargs)
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

    return _Config(serving_type=serving_type, h5_path=h5_path, script_path=script_path, export_path=export_path,
                   onnx_path=onnx_path, input_layer_names=input_layer_names, output_layer_names=output_layer_names,
                   input_signatures=input_signatures, output_signatures=output_signatures, model_name=model_name,
                   max_batch_size=max_batch_size, job_id=job_id, callback=callback, checkpoint_path=checkpoint_path,
                   frozen_graph_path=frozen_graph_path, input_names=input_names, input_formats=input_formats,
                   output_names=output_names)


def _create_config_from_json(json_message):
    if isinstance(json_message, str):
        json_message = json.loads(json_message)
    return _Config(**json_message)


def create_config(from_source='env', json_message=None) -> object:
    """
    Create message config, default from environment variables
    :param from_source: from where parse message, e.g. 'env' or 'json'
    :param json_message: json message
    :return:
    """
    if from_source == 'env':
        return _create_config_from_env()
    else:
        return _create_config_from_json(json_message)
