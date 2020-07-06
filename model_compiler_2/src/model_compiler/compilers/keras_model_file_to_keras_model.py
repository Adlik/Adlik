# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import importlib.util

import tensorflow as tf
from tensorflow import keras

from . import repository
from ..models.irs.keras_model import KerasModel
from ..models.sources.keras_model_file import KerasModelFile


def _load_module(file_path, name):
    spec = importlib.util.spec_from_file_location(name=name, location=file_path)
    module = importlib.util.module_from_spec(spec=spec)

    spec.loader.exec_module(module)

    return module


def _get_custom_objects(file_path):
    script_module_name = ''
    custom_objects = {}

    module = _load_module(file_path, script_module_name)

    for name, value in vars(module).items():
        if isinstance(value, type) and value.__module__ == script_module_name:
            if issubclass(value, keras.layers.Layer):
                custom_objects[name] = value

    return custom_objects


@repository.REPOSITORY.register(source_type=KerasModelFile, target_type=KerasModel)
def compile_source(source: KerasModelFile) -> KerasModel:
    with tf.Graph().as_default():
        if source.script_path:
            with tf.compat.v1.Session(graph=tf.Graph()):
                custom_objects = _get_custom_objects(source.script_path)

            with tf.compat.v1.Session().as_default() as session:
                model = keras.models.load_model(source.model_path, custom_objects=custom_objects, compile=False)
        else:
            with tf.compat.v1.Session().as_default() as session:
                model = keras.models.load_model(source.model_path, custom_objects=None, compile=False)

    return KerasModel(model=model, session=session)
