# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from ..models.sources.keras_model_file import KerasModelFile

from ..models.targets.tflite_model import TfLiteModel
from .. import tflite_util
from .. import keras_util


@repository.REPOSITORY.register(source_type=KerasModelFile, target_type=TfLiteModel, config_type=tflite_util.Config)
def compile_source(source: KerasModelFile, config: tflite_util.Config) -> TfLiteModel:
    if source.script_path:
        custom_objects = keras_util.get_custom_objects(source.script_path)
    else:
        custom_objects = None

    model = tf.keras.models.load_model(filepath=source.model_path, custom_objects=custom_objects, compile=False)
    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    # tensorflow>=2.7.0, if batch size is -1, to avoid using tf_select_ops
    converter._experimental_default_to_single_batch_in_tensor_list_ops = True  # pylint: disable=protected-access
    tflite_model = tflite_util.get_tflite_model(converter, config)
    return TfLiteModel(tflite_model, config.input_formats)
