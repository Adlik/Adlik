# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from types import ModuleType

from . import caffe_model_file_to_onnx_model, keras_model_file_to_keras_model, keras_model_file_to_tflite_model, \
    keras_model_to_tf_model, mxnet_model_file_to_onnx_model, onnx_model_file_to_onnx_model, \
    onnx_model_to_openvino_model, onnx_model_to_tflite_model, saved_model_file_to_openvino_model, \
    saved_model_file_to_saved_model, saved_model_file_to_tflite_model, saved_model_to_openvino_model, \
    saved_model_to_tflite_model, tf_frozen_graph_model_file_to_openvino_model, tf_frozen_graph_model_file_to_tf_model, \
    tf_frozen_graph_model_to_onnx_model, tf_model_file_to_tf_model, tf_model_to_saved_model, \
    tf_model_to_tf_frozen_graph_model, torch_model_file_to_onnx_model

try:
    from . import onnx_model_to_tensorrt_model
except ImportError:  # pragma: no cover
    onnx_model_to_tensorrt_model = ModuleType('model_compiler.compilers.onnx_model_to_tensorrt_model')

__all__ = [
    'caffe_model_file_to_onnx_model',
    'keras_model_file_to_keras_model',
    'keras_model_file_to_tflite_model',
    'keras_model_to_tf_model',
    'mxnet_model_file_to_onnx_model',
    'onnx_model_file_to_onnx_model',
    'onnx_model_to_openvino_model',
    'onnx_model_to_tensorrt_model',
    'onnx_model_to_tflite_model',
    'saved_model_file_to_openvino_model',
    'saved_model_file_to_saved_model',
    'saved_model_file_to_tflite_model',
    'saved_model_to_openvino_model',
    'saved_model_to_tflite_model',
    'tf_frozen_graph_model_file_to_openvino_model',
    'tf_frozen_graph_model_file_to_tf_model',
    'tf_frozen_graph_model_to_onnx_model',
    'tf_model_file_to_tf_model',
    'tf_model_to_saved_model',
    'tf_model_to_tf_frozen_graph_model',
    'torch_model_file_to_onnx_model'
]
