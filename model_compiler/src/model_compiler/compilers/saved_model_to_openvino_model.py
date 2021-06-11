# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory

from . import repository
from ..models.targets.openvino_model import OpenvinoModel
from ..models.targets.saved_model import SavedModel
from ..openvino_util import Config, execute_optimize_action


def _get_optimize_params(input_model, output_dir, config, inputs, outputs):
    params = {'script_name': 'mo_tf.py',
              'model_name': 'model',
              'saved_model_dir': input_model,
              'output_dir': output_dir,
              'input': ','.join(item.name.split(':')[0] for item in inputs),
              'output': ','.join(item.name.split(':')[0] for item in outputs)}
    # if enble_nhwc_to_nchw=None or False set --disable_nhwc_to_nchw
    if not config.enable_nhwc_to_nchw:
        params['disable_nhwc_to_nchw'] = None
    if config.input_shapes is not None:
        params['input_shape'] = ','.join(str(shape) for shape in config.input_shapes)
    if config.max_batch_size is not None:
        params['batch'] = str(config.max_batch_size)
    if config.data_type is not None:
        params['data_type'] = config.data_type
    return params


@repository.REPOSITORY.register(source_type=SavedModel, target_type=OpenvinoModel, config_type=Config)
def compile_source(source: SavedModel, config: Config) -> OpenvinoModel:
    with TemporaryDirectory() as directory:
        source.save(directory)
        openvino_temp_path = TemporaryDirectory()
        optimize_params = _get_optimize_params(directory,
                                               openvino_temp_path.name,
                                               config,
                                               source.get_inputs(),
                                               source.get_outputs())
        execute_optimize_action(optimize_params)
    return OpenvinoModel.from_directory(openvino_temp_path)
