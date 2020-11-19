# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Resnet50 model
"""

import tensorflow as tf
import os


def _get_model():
    return tf.keras.applications.resnet50.ResNet50(include_top=True, weights='imagenet',
                                                   input_tensor=None, input_shape=None,
                                                   pooling=None,
                                                   classes=1000)


def main():
    model = _get_model()
    save_path = os.path.join(os.path.dirname(__file__), 'model', 'resnet50.h5')
    print(save_path)
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    model.save(save_path)


if __name__ == '__main__':
    main()
