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
    if config.signature_keys:
        signature_keys = config.signature_keys
    else:
        signature_keys = tf.saved_model.load(source.model_path, tags=['serve']).signatures

    converter = tf.lite.TFLiteConverter.from_saved_model(source.model_path, signature_keys=signature_keys)
    # tensorflow>=2.7.0, if batch size is -1, to avoid using tf_select_ops
    converter._experimental_default_to_single_batch_in_tensor_list_ops = True  # pylint: disable=protected-access
    tflite_model = get_tflite_model(converter, config)

    return TfLiteModel(tflite_model, config.input_formats)
