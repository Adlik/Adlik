# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.saved_model_to_tflite_model as compiler
from model_compiler.models.targets.saved_model import DataFormat, Input, Output, SavedModel


def _make_saved_model() -> SavedModel:
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
        weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
        output_z = input_x + input_y + weight

        session.run(weight.initializer)

    return SavedModel(inputs=[Input(name='x', tensor=input_x, data_format=DataFormat.CHANNELS_FIRST),
                              Input(name='y', tensor=input_y, data_format=DataFormat.CHANNELS_LAST)],
                      outputs=[Output(name='z', tensor=output_z)],
                      session=session)


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        compiled = compiler.compile_source(_make_saved_model())

        self.assertIsInstance(compiled.tflite_model, bytes)
        self.assertEqual(compiled.input_formats, [DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST])
