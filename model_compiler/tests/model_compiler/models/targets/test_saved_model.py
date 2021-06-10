# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import tensorflow as tf

from model_compiler.models.targets.saved_model import Input, Output, SavedModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_saved_model() -> SavedModel:
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
        weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
        output_z = input_x + input_y + weight

        session.run(weight.initializer)

    return SavedModel(inputs=[Input(name='x', tensor=input_x), Input(name='y', tensor=input_y)],
                      outputs=[Output(name='z', tensor=output_z)],
                      session=session)


def _make_tf2_saved_model() -> SavedModel:
    origin_model = tf.keras.Sequential()
    origin_model.add(tf.keras.layers.Dense(units=8, name='l1', input_shape=(16,)))
    origin_model.add(tf.keras.layers.Dense(units=4, name='l2'))
    origin_model.add(tf.keras.layers.Dense(units=2, name='l3'))
    with TemporaryDirectory() as directory:
        tf.saved_model.save(origin_model, directory)
        model = tf.saved_model.load(directory)
        infer = model.signatures['serving_default']

    return SavedModel(inputs=[Input(name='l1_input',
                                    tensor=infer.structured_input_signature[1]['l1_input'])],
                      outputs=[Output(name='l3',
                                      tensor=infer.structured_outputs['l3'])],
                      session=None,
                      model=model)


class KerasModelFileTestCase(TestCase):
    def test_get_inputs(self):
        saved_model = _make_saved_model()

        self.assertEqual(saved_model.get_inputs(),
                         [ModelInput(name='x', data_type=tf.float32.as_datatype_enum, format=None, dims=[4]),
                          ModelInput(name='y', data_type=tf.float32.as_datatype_enum, format=None, dims=[4])])

    def test_get_outputs(self):
        saved_model = _make_saved_model()

        self.assertEqual(saved_model.get_outputs(),
                         [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[4])])

    def test_save(self):
        saved_model = _make_saved_model()

        with TemporaryDirectory() as save_path:
            saved_model.save(save_path)

            self.assertEqual(sorted(os.listdir(save_path)), ['saved_model.pb', 'variables'])

            self.assertEqual(sorted(os.listdir(os.path.join(save_path, 'variables'))),
                             ['variables.data-00000-of-00001', 'variables.index'])

    def test_get_platform(self):
        self.assertEqual(SavedModel.get_platform(), ('tensorflow', tf.version.VERSION))

    def test_tf2_saved_model(self):
        saved_model = _make_tf2_saved_model()

        with TemporaryDirectory() as save_path:
            saved_model.save(save_path)

            self.assertEqual(sorted(os.listdir(save_path)), ['assets', 'saved_model.pb', 'variables'])

            self.assertEqual(sorted(os.listdir(os.path.join(save_path, 'variables'))),
                             ['variables.data-00000-of-00001', 'variables.index'])
