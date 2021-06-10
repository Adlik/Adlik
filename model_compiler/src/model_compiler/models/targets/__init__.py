# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from types import ModuleType

from . import saved_model, tflite_model

try:
    from . import tensorrt_model
except ImportError:  # pragma: no cover
    tensorrt_model = ModuleType('model_compiler.models.targets.tensorrt_model')

__all__ = [
    'onnx_model',
    'openvino_model',
    'saved_model',
    'tensorrt_model',
    'tflite_model'
]
