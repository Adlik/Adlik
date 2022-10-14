# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from tempfile import TemporaryDirectory

import tensorflow as tf

from . import repository
from ..tflite_util import Config, get_tflite_model
from ..models.targets.saved_model import SavedModel
from ..models.targets.tflite_model import TfLiteModel


@repository.REPOSITORY.register(source_type=SavedModel, target_type=TfLiteModel, config_type=Config)
def compile_source(source: SavedModel, config: Config) -> TfLiteModel:
    with TemporaryDirectory() as directory:
        source.save(directory)

        converter = tf.lite.TFLiteConverter.from_saved_model(directory)
        # tensorflow>=2.7.0, if batch size is -1, to avoid using tf_select_ops
        converter._experimental_default_to_single_batch_in_tensor_list_ops = True  # pylint: disable=protected-access
        tflite_model = get_tflite_model(converter, config)
        input_formats = [model_input.data_format for model_input in source.inputs]

        return TfLiteModel(tflite_model, input_formats)
