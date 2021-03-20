# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf
from tensorflow import keras

from . import repository
from .. import utilities
from .. import keras_util
from ..models.irs.keras_model import KerasModel
from ..models.sources.keras_model_file import KerasModelFile


@repository.REPOSITORY.register(source_type=KerasModelFile, target_type=KerasModel)
def compile_source(source: KerasModelFile) -> KerasModel:
    with tf.Graph().as_default():
        if source.script_path:
            with tf.compat.v1.Session(graph=tf.Graph(), config=utilities.get_tf_cpu_only_config()):
                custom_objects = keras_util.get_custom_objects(source.script_path)
        else:
            custom_objects = None

        with tf.compat.v1.Session(config=utilities.get_tf_cpu_only_config()).as_default() as session:
            keras.backend.set_learning_phase(0)
            model = keras.models.load_model(source.model_path, custom_objects=custom_objects, compile=False)

    return KerasModel(model=model, session=session)
