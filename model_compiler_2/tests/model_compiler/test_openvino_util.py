# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import contextlib
import os
from os import path
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import tensorflow as tf

from model_compiler import openvino_util
from model_compiler.openvino_util import Layer, ModelParser


def _set_env(env, key, value):
    if value is None:
        try:
            del env[key]
        except KeyError:
            pass
    else:
        env[key] = value


@contextlib.contextmanager
def _use_cvsdk_dir(dir_path):
    env = os.environ
    key = 'INTEL_CVSDK_DIR'
    saved = os.getenv(key)

    _set_env(env, key, dir_path)

    try:
        yield
    finally:
        _set_env(env, key, saved)


class OpenVinoUtilTestCase(TestCase):
    def test_set_env(self):
        env = {'foo': 'bar'}

        _set_env(env=env, key='def', value=None)

        self.assertEqual(env, {'foo': 'bar'})

        _set_env(env=env, key='def', value='ghi')

        self.assertEqual(env, {'foo': 'bar', 'def': 'ghi'})

        _set_env(env=env, key='foo', value=None)

        self.assertEqual(env, {'def': 'ghi'})

    def test_get_version_no_file(self):
        with TemporaryDirectory() as cvsdk_dir, _use_cvsdk_dir(cvsdk_dir):
            result = openvino_util.get_version()

        self.assertEqual(result, 'unknown version')

    def test_get_version_custom_version(self):
        with TemporaryDirectory() as cvsdk_dir:
            model_optimizer_dir = path.join(cvsdk_dir, 'deployment_tools', 'model_optimizer')

            os.makedirs(model_optimizer_dir)

            with open(path.join(model_optimizer_dir, 'version.txt'), 'w') as version_file:
                version_file.writelines('foo bar')

            with _use_cvsdk_dir(cvsdk_dir):
                result = openvino_util.get_version()

        self.assertEqual(result, 'foo bar')


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
    params = {'script_name': 'mo_tf.py',
              'model_name': 'model',
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
            model_parser = ModelParser.from_xml(os.path.join(temp_path.name, 'model.xml'))
            layer = next(layer for layer in model_parser.layers if layer.type == 'Result')
            fake_layer = Layer(ports=layer.ports, type=layer.type, id='xxx', name=layer.name)
            model_parser.layers.append(fake_layer)
            with self.assertRaises(ValueError):
                model_parser.get_outputs()
