# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
import tensorflow as tf
from onnx_tf.backend import prepare

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.targets.tflite_model import TfLiteModel


@repository.REPOSITORY.register(source_type=OnnxModel, target_type=TfLiteModel)
def compile_source(source: OnnxModel) -> TfLiteModel:
    tf_representation = prepare(source.model_proto)
    with TemporaryDirectory() as directory:
        tf_representation.export_graph(directory)
        converter = tf.lite.TFLiteConverter.from_saved_model(directory)
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS]
        # tensorflow>=2.7.0, if batch size is -1, to avoid using tf_select_ops
        converter._experimental_default_to_single_batch_in_tensor_list_ops = True  # pylint: disable=protected-access
        tflite_model = converter.convert()

        return TfLiteModel(tflite_model, source.input_data_formats)
