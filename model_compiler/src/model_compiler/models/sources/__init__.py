# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from . import keras_model_file, onnx_model_file, openvino_model_file, \
              tf_frozen_graph_file, tf_model_file, saved_model_file, torch_model_file

__all__ = [
    'caffe_model_file',
    'keras_model_file',
    'onnx_model_file',
    'openvino_model_file',
    'paddle_model_file',
    'saved_model_file',
    'tf_frozen_graph_file',
    'tf_model_file',
    'torch_model_file'

]
