# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=import-outside-toplevel

import tensorflow as tf

from . import repository
from ..models.sources.keras_model_file import KerasModelFile
from ..models.targets.tvm_model import TvmModel, Input, Output
from ..keras_util import Config, get_inputs, get_outputs, DataFormat


def _get_shape_dict(model_inputs, max_batch_size):
    shape_dict = {}
    for input_tensor, data_format in model_inputs:
        tensor_shape = list(input_tensor.shape)
        tensor_shape.pop(0)
        tensor_shape.insert(0, max_batch_size)
        if data_format == DataFormat.CHANNELS_LAST:
            tensor_shape[1], tensor_shape[3] = tensor_shape[3], tensor_shape[1]
        shape_dict[input_tensor.name] = tensor_shape
    return shape_dict


@repository.REPOSITORY.register(source_type=KerasModelFile, target_type=TvmModel, config_type=Config)
def compile_source(source: KerasModelFile, config: Config) -> TvmModel:
    import tvm.relay as relay
    from ..tvm_utils import compile_relay as tvm_compile

    tf.keras.backend.set_learning_phase(0)
    source_model = tf.keras.models.load_model(source.model_path, compile=False)
    model_inputs = get_inputs(source_model, config.input_nodes)

    shape_dict = _get_shape_dict(model_inputs, config.max_batch_size)
    model, params = relay.frontend.from_keras(source_model, shape_dict)
    compiled_lib = tvm_compile(model, params, config, shape_dict)
    return TvmModel(tvm_model=compiled_lib,
                    model_inputs=[Input(name=tensor.name,
                                        shape=shape_dict[tensor.name],
                                        data_type=tensor.dtype.as_datatype_enum,
                                        data_format=DataFormat.CHANNELS_FIRST) for tensor, _ in model_inputs],
                    model_outputs=[Output(name=tensor.name,
                                          shape=list(tensor.shape),
                                          data_type=tensor.dtype.as_datatype_enum)
                                   for tensor in get_outputs(source_model, config.output_nodes)])
