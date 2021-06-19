# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory

from . import repository
from ..models.sources.saved_model_file import SavedModelFile
from ..models.targets.openvino_model import OpenvinoModel
from ..openvino_util import Config, execute_optimize_action


def _get_optimize_params(input_model, output_dir, config):
    params = {'script_name': 'mo_tf.py',
              'model_name': 'model',
              'saved_model_dir': input_model,
              'output_dir': output_dir}
    # if enble_nhwc_to_nchw=None or False set --disable_nhwc_to_nchw
    if not config.enable_nhwc_to_nchw:
        params['disable_nhwc_to_nchw'] = None
    if config.input_names is not None:
        params['input'] = ','.join(config.input_names)
    if config.input_shapes is not None:
        params['input_shape'] = ','.join(str(shape) for shape in config.input_shapes)
    if config.max_batch_size is not None:
        params['batch'] = str(config.max_batch_size)
    if config.output_names is not None:
        params['output'] = ','.join(config.output_names)
    if config.saved_model_tags is None:
        params['saved_model_tags'] = 'serve'
    else:
        params['saved_model_tags'] = ','.join(config.saved_model_tags)
    if config.data_type is not None:
        params['data_type'] = config.data_type
    return params


@repository.REPOSITORY.register(source_type=SavedModelFile, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: SavedModelFile, config: Config) -> object:
    temp_path = TemporaryDirectory()
    optimize_params = _get_optimize_params(source.model_path, temp_path.name, config)
    execute_optimize_action(optimize_params)
    return OpenvinoModel.from_directory(temp_path)
