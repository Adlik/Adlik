# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.saved_model_to_openvino_model as compiler
from model_compiler.compilers.saved_model_to_openvino_model import Config
from model_compiler.models.targets.saved_model import Input, Output, SavedModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_saved_model() -> SavedModel:
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 2, 3, 4], name='y')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        output_z = tf.multiply(input_x + input_y, weight, name='z')
        session.run(weight.initializer)

    return SavedModel(inputs=[Input(name='x', tensor=input_x, data_format=None),
                              Input(name='y', tensor=input_y, data_format=None)],
                      outputs=[Output(name='z', tensor=output_z)],
                      session=session)


class CompileSourceTestCase(TestCase):
    def test_compile_with_no_params(self):
        config = Config.from_json({'max_batch_size': 1})
        compiled = compiler.compile_source(source=_make_saved_model(), config=config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                          ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_all_params_with_shape(self):
        config = Config.from_json({'input_names': ['x', 'y'],
                                   'input_shapes': [[1, 2, 3, 4], [1, 2, 3, 4]],
                                   'output_names': ['z'],
                                   'enable_nhwc_to_nchw': False})
        compiled = compiler.compile_source(source=_make_saved_model(), config=config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4]),  # pylint: disable=no-member
                          ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[2, 3, 4])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[2, 3, 4])])

    def test_compile_with_all_params_with_enable_nhwc_to_nchw_true(self):
        config = Config.from_json({'input_names': ['x', 'y'],
                                   'output_names': ['z'],
                                   'max_batch_size': 1,
                                   'enable_nhwc_to_nchw': True})
        compiled = compiler.compile_source(source=_make_saved_model(), config=config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[4, 2, 3]),  # pylint: disable=no-member
                          ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[4, 2, 3])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[4, 2, 3])])
