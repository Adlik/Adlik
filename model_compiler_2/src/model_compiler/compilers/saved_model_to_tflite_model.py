# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory
import tensorflow as tf

from . import repository
from ..models.targets.saved_model import SavedModel
from ..models.targets.tflite_model import TfLiteModel


@repository.REPOSITORY.register(source_type=SavedModel, target_type=TfLiteModel)
def compile_source(source: SavedModel) -> TfLiteModel:
    with TemporaryDirectory() as directory:
        source.save(directory)
        with source.session.as_default():
            converter = tf.lite.TFLiteConverter.from_saved_model(directory)
    tflite_model = converter.convert()

    input_formats = [model_input.data_format for model_input in source.inputs]
    return TfLiteModel(tflite_model, input_formats)
