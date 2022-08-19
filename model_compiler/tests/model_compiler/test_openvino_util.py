# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import tensorflow as tf
from model_compiler import openvino_util


def _save_frozen_graph_model(model_file):
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        tf.multiply(input_x + input_y, weight, name='z')
        session.run(weight.initializer)
        constant_graph = tf.compat.v1.graph_util.convert_variables_to_constants(session, session.graph_def, ['z'])

    with open(model_file.name, mode='wb') as graph_file:
        graph_file.write(constant_graph.SerializeToString())


def _get_optimize_params(input_model, output_dir):
    params = {'model_name': 'model',
              'input_model': input_model,
              'output_dir': output_dir,
              'batch': '1',
              'input': 'x,y',
              'output': 'z',
              'disable_nhwc_to_nchw': None}
    return params


class ModelParserTestCase(TestCase):
    def test_invalid_layer_id(self):
        with NamedTemporaryFile(suffix='.pb') as pb_model_file:
            _save_frozen_graph_model(pb_model_file)
            temp_path = TemporaryDirectory()
            optimize_params = _get_optimize_params(pb_model_file.name, temp_path.name)
            openvino_util.execute_optimize_action(optimize_params)
            model_parser = openvino_util.ModelParser.from_model(os.path.join(temp_path.name, 'model.xml'))
            with self.assertRaises(ValueError):
                model_parser.get_outputs()
