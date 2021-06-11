# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import NamedTuple, Optional, Sequence, Tuple

import tensorflow as tf
from tensorflow.core.framework import types_pb2  # pylint: disable=no-name-in-module
from tensorflow.python.compiler.tensorrt import trt_convert as trt  # pylint: disable=no-name-in-module

from .. import data_format, repository
from ..data_format import DataFormat
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


class Input(NamedTuple):
    name: str
    tensor: tf.Tensor
    data_format: Optional[DataFormat] = None


class Output(NamedTuple):
    name: str
    tensor: tf.Tensor


@repository.REPOSITORY.register_target_model('tftrt')
class TfTRTSavedModel(NamedTuple):
    tftrt_saved_model: trt.TrtGraphConverterV2
    inputs: Sequence[Input]
    outputs: Sequence[Output]

    def get_inputs(self) -> Sequence[ModelInput]:
        return [ModelInput(name=item.name,
                           data_type=types_pb2.DataType.items()[item.tensor.dtype][0],
                           format=data_format.as_model_config_data_format(item.data_format),
                           dims=[-1 if dim is None else dim.size for dim in item.tensor.tensor_shape.dim[1:]])
                for item in self.inputs]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=item.name,
                            data_type=types_pb2.DataType.items()[item.tensor.dtype][0],
                            dims=[-1 if dim is None else dim.size for dim in item.tensor.tensor_shape.dim[1:]])
                for item in self.outputs]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        self.tftrt_saved_model.save(path)

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        import tensorrt  # pylint: disable=import-outside-toplevel

        return 'tensorflow-tensorrt', tf.version.VERSION + '-' + tensorrt.__version__
