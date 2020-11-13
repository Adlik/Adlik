# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
InceptionV3 model
"""

from keras.applications.inception_v3 import InceptionV3
import os


def get_model():
    return InceptionV3(weights='imagenet')


def main():
    model = get_model()
    save_path = os.path.join(os.path.dirname(__file__), 'model', 'inceptionv3.h5')
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    model.save(save_path)


if __name__ == '__main__':
    main()
