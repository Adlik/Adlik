# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.tf_model_file_to_tf_model as compiler
from model_compiler.compilers.tf_model_file_to_tf_model import Config
from model_compiler.models.irs.tf_model import DataFormat
from model_compiler.models.sources.tf_model_file import TfModelFile


def _save_tensorflow_model(model_path):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        tf.multiply(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        tf.compat.v1.train.Saver().save(session, model_path)


class CompileSourceTestCase(TestCase):
    def test_compile_with_variables(self):
        with TemporaryDirectory() as directory:
            model_path = os.path.join(directory, 'model.ckpt')

            _save_tensorflow_model(model_path)

            config = Config.from_json({'input_names': ['x:0', 'y:0'],
                                       'output_names': ['z:0'],
                                       'input_formats': ['channels_first']})

            compiled = compiler.compile_source(TfModelFile(model_path=model_path), config)

        self.assertIsInstance(compiled.session, tf.compat.v1.Session)
        self.assertEqual([model_input.tensor.name for model_input in compiled.inputs], ['x:0', 'y:0'])

        self.assertEqual([model_input.data_format for model_input in compiled.inputs],
                         [DataFormat.CHANNELS_FIRST, None])

        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z:0'])

    def test_compile_with_none_input_format(self):
        with TemporaryDirectory() as directory:
            model_path = os.path.join(directory, 'model.ckpt')

            _save_tensorflow_model(model_path)

            config = Config.from_json({'input_names': ['x:0', 'y:0'],
                                       'output_names': ['z:0'],
                                       'input_formats': []})

            compiled = compiler.compile_source(TfModelFile(model_path=model_path), config)

        self.assertEqual([model_input.data_format for model_input in compiled.inputs], [None, None])

    def test_compile_with_op_name(self):
        with TemporaryDirectory() as directory:
            model_path = os.path.join(directory, 'model.ckpt')

            _save_tensorflow_model(model_path)

            config = Config.from_json({'input_names': ['x', 'y'],
                                       'output_names': ['z'],
                                       'input_formats': ['channels_first', 'channels_last']})

            compiled = compiler.compile_source(TfModelFile(model_path=model_path), config)

        self.assertIsInstance(compiled.session, tf.compat.v1.Session)
        self.assertEqual([model_input.tensor.name for model_input in compiled.inputs], ['x:0', 'y:0'])

        self.assertEqual([model_input.data_format for model_input in compiled.inputs],
                         [DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST])

        self.assertEqual([model_output.name for model_output in compiled.outputs], ['z:0'])
