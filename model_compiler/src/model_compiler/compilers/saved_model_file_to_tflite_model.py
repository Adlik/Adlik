# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from ..models.sources.saved_model_file import SavedModelFile
from ..models.targets.tflite_model import TfLiteModel

from ..tflite_util import Config
from ..tflite_util import get_tflite_model


@repository.REPOSITORY.register(source_type=SavedModelFile, target_type=TfLiteModel, config_type=Config)
def compile_source(source: SavedModelFile, config: Config) -> TfLiteModel:
    converter = tf.lite.TFLiteConverter.from_saved_model(source.model_path)
    tflite_model = get_tflite_model(converter, config)

    return TfLiteModel(tflite_model, config.input_formats)
