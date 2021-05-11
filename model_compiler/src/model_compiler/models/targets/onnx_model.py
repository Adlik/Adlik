# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from typing import NamedTuple, Optional, Sequence, Tuple
import onnx
from onnx import ModelProto

from .. import data_format, repository
from ..data_format import DataFormat
from ..data_type import DataType
from ...protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _onnx_dtype_to_tf_dtype(onnx_dtype):
    return DataType.from_onnx_data_type(onnx_dtype).to_tf_data_type()


def _get_onnx_model_layer_dims(layer):
    return [-1 if dim.dim_value is None else dim.dim_value for dim in layer.type.tensor_type.shape.dim[1:]]


@repository.REPOSITORY.register_target_model('onnx')
class ONNXModel(NamedTuple):
    onnx_model: ModelProto
    input_formats: Sequence[Optional[DataFormat]]

    def get_inputs(self) -> Sequence[ModelInput]:
        graph = self.onnx_model.graph
        initializers = {initializer.name for initializer in graph.initializer}

        model_input = [input_spec for input_spec in graph.input if input_spec.name not in initializers]

        return [ModelInput(name=model_input.name,
                           data_type=_onnx_dtype_to_tf_dtype(model_input.type.tensor_type.elem_type),
                           format=data_format.as_model_config_data_format(input_format),
                           dims=_get_onnx_model_layer_dims(model_input))
                for model_input, input_format in zip(model_input, self.input_formats)]

    def get_outputs(self) -> Sequence[ModelOutput]:
        return [ModelOutput(name=model_output.name,
                            data_type=_onnx_dtype_to_tf_dtype(model_output.type.tensor_type.elem_type),
                            dims=_get_onnx_model_layer_dims(model_output))
                for model_output in self.onnx_model.graph.output]

    def save(self, path: str) -> None:
        os.makedirs(path, exist_ok=True)

        onnx.save(self.onnx_model, os.path.join(path, 'model.onnx'))

    @staticmethod
    def get_platform() -> Tuple[str, str]:
        return 'onnx', onnx.version.version
