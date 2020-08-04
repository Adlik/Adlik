# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import numpy
import tensorflow as tf

import model_compiler.compilers.saved_model_to_tflite_model as compiler
from model_compiler.compilers.saved_model_to_tflite_model import Config
from model_compiler.models.targets.saved_model import DataFormat, Input, Output, SavedModel


def _make_saved_model() -> SavedModel:
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 256])
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 256])
        weight = tf.Variable(initial_value=numpy.arange(256, dtype=numpy.float32), dtype=tf.float32)
        output_z = input_x + input_y + weight

        session.run(weight.initializer)

    return SavedModel(inputs=[Input(name='x', tensor=input_x, data_format=DataFormat.CHANNELS_FIRST),
                              Input(name='y', tensor=input_y, data_format=DataFormat.CHANNELS_LAST)],
                      outputs=[Output(name='z', tensor=output_z)],
                      session=session)


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config(optimization=False, supported_types=None), Config.from_json({}))
        self.assertEqual(Config(optimization=False, supported_types=None), Config.from_json({'optimization': False}))
        self.assertEqual(Config(optimization=True, supported_types=None), Config.from_json({'optimization': True}))

        self.assertEqual(Config(optimization=False, supported_types=[tf.float16]),
                         Config.from_json({'supported_types': ['float16']}))

        self.assertEqual(Config(optimization=False, supported_types=[tf.float16, tf.float32]),
                         Config.from_json({'supported_types': ['float16', 'float32']}))

        self.assertEqual(Config(optimization=True, supported_types=[tf.float16, tf.float32]),
                         Config.from_json({'optimization': True, 'supported_types': ['float16', 'float32']}))

    def test_from_env(self):
        self.assertEqual(Config(optimization=False, supported_types=None), Config.from_env({}))
        self.assertEqual(Config(optimization=False, supported_types=None), Config.from_env({'OPTIMIZATION': '0'}))
        self.assertEqual(Config(optimization=True, supported_types=None), Config.from_env({'OPTIMIZATION': '1'}))

        self.assertEqual(Config(optimization=False, supported_types=[tf.float16]),
                         Config.from_env({'SUPPORTED_TYPES': 'float16'}))

        self.assertEqual(Config(optimization=False, supported_types=[tf.float16, tf.float32]),
                         Config.from_env({'SUPPORTED_TYPES': 'float16,float32'}))

        self.assertEqual(Config(optimization=True, supported_types=[tf.float16, tf.float32]),
                         Config.from_env({'OPTIMIZATION': '1', 'SUPPORTED_TYPES': 'float16,float32'}))

    def test_invalid_data_type(self):
        with self.assertRaises(ValueError):
            Config.from_json({'supported_types': ['foobar']})

        with self.assertRaises(ValueError):
            Config.from_json({'supported_types': ['as_dtype']})


class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        compiled = compiler.compile_source(source=_make_saved_model(), config=Config())

        self.assertIsInstance(compiled.tflite_model, bytes)
        self.assertEqual(compiled.input_formats, [DataFormat.CHANNELS_FIRST, DataFormat.CHANNELS_LAST])

    def test_compile_simple_fp16(self):
        compiled_1 = compiler.compile_source(source=_make_saved_model(), config=Config())

        compiled_2 = compiler.compile_source(source=_make_saved_model(),
                                             config=Config(optimization=True, supported_types=[tf.float16]))

        self.assertLess(len(compiled_2.tflite_model), len(compiled_1.tflite_model))
