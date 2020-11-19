# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import NamedTemporaryFile, TemporaryDirectory
from unittest import TestCase

import tensorflow as tf

from model_compiler.models.targets.openvino_model import OpenvinoModel
from model_compiler.openvino_util import execute_optimize_action
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


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


def _make_openvino_model(pb_model_file):
    temp_path = TemporaryDirectory()
    optimize_params = _get_optimize_params(pb_model_file.name, temp_path.name)
    execute_optimize_action(optimize_params)
    return OpenvinoModel.from_directory(temp_path)


class FrozenGraphFileTestCase(TestCase):
    def test_get_inputs(self):
        with NamedTemporaryFile(suffix='.pb') as pb_model_file:
            _save_frozen_graph_model(pb_model_file)
            openvino_model = _make_openvino_model(pb_model_file)
            self.assertEqual(openvino_model.get_inputs(),
                             [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member

    def test_get_outputs(self):
        with NamedTemporaryFile(suffix='.pb') as pb_model_file:
            _save_frozen_graph_model(pb_model_file)
            openvino_model = _make_openvino_model(pb_model_file)
            self.assertEqual(openvino_model.get_outputs(),
                             [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_save(self):
        with TemporaryDirectory() as save_path, NamedTemporaryFile(suffix='.pb') as pb_model_file:
            _save_frozen_graph_model(pb_model_file)
            openvino_model = _make_openvino_model(pb_model_file)
            test_save_model_path = os.path.join(save_path, 'test')
            openvino_model.save(test_save_model_path)
            self.assertEqual(sorted(os.listdir(test_save_model_path)), ['model.bin', 'model.mapping', 'model.xml'])

    def test_get_platform(self):
        platform, version = OpenvinoModel.get_platform()
        self.assertEqual(platform, 'openvino')
        self.assertNotEqual(version, '')
