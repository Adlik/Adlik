# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.onnx_model import ONNXModel


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=ONNXModel)
def compile_source(source: OnnxModel) -> ONNXModel:
    return ONNXModel(onnx_model=source.model_proto,
                     input_formats=source.input_data_formats,
                     model_inputs=source.get_inputs())
