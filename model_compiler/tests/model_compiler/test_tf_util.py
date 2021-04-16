# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf

from model_compiler.tensorflow_util import Config, get_inputs
from model_compiler.models.data_format import DataFormat


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_names': ['input'],
                                           'output_names': ['output'],
                                           'input_formats': ['channels_first']}),
                         Config(input_names=['input'],
                                data_formats=[DataFormat.CHANNELS_FIRST],
                                output_names=['output']))

    def test_from_json_no_names(self):
        self.assertEqual(Config.from_json({}),
                         Config(input_names=None,
                                data_formats=[],
                                output_names=None))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_NAMES': 'input1,input2:0',
                                          'OUTPUT_NAMES': 'output',
                                          'INPUT_FORMATS': 'channels_last'}),
                         Config(input_names=['input1', 'input2:0'],
                                data_formats=[DataFormat.CHANNELS_LAST],
                                output_names=['output']))

    def test_from_env_no_names(self):
        self.assertEqual(Config.from_env({}),
                         Config(input_names=None,
                                data_formats=[],
                                output_names=None))


class TestGetInputs(TestCase):
    def test_get_inputs(self):
        with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 256])
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 256])
            config = Config.from_json({'input_formats': ['channels_first']})
            model_information = get_inputs(session.graph, config)
        self.assertEqual([[model_input.name, input_format] for model_input, input_format in model_information],
                         [[input_x.name, DataFormat.CHANNELS_FIRST], [input_y.name, None]])
