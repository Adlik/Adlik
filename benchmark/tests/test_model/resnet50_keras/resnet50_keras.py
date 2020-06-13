# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Resnet50 model
"""

from keras.applications.resnet50 import ResNet50
import os


def get_model():
    return ResNet50(weights='imagenet')


def main():
    model = get_model()
    save_path = os.path.join(os.path.dirname(__file__), 'model', 'resnet50.h5')
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    model.save(save_path)


if __name__ == '__main__':
    main()
