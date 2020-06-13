# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Resnet50 model
"""

import tensorflow.compat.v1 as tf
import os


def _get_model():
    return tf.keras.applications.resnet50.ResNet50(weights='imagenet')


def main():
    with tf.Session() as sess:
        model = _get_model()
        print(model.output_names)
        sess.run(tf.global_variables_initializer())
        save_path = os.path.join(os.path.dirname(__file__), 'model', 'resnet50.ckpt')
        dir_name = os.path.dirname(save_path)
        os.makedirs(dir_name, exist_ok=True)
        tf.train.Saver().save(sess, save_path)


if __name__ == '__main__':
    main()
