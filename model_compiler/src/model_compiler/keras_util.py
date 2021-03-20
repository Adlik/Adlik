# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import importlib.util

from tensorflow import keras


def _load_module(file_path, name):
    spec = importlib.util.spec_from_file_location(name=name, location=file_path)
    module = importlib.util.module_from_spec(spec=spec)

    spec.loader.exec_module(module)

    return module


def get_custom_objects(file_path):
    module_name = ''
    custom_objects = {}
    module = _load_module(file_path, module_name)

    for name, value in vars(module).items():
        if isinstance(value, type) and issubclass(value, keras.layers.Layer) and value.__module__ == module_name:
            custom_objects[name] = value

    return custom_objects
