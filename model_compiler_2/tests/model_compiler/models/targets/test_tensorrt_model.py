# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import pytest
import tensorflow as tf
import tensorflow.python.eager.context as eager_context
import tensorrt

import model_compiler.compilers.repository as compiler_repository
from model_compiler.models.irs.tf_model import Input as TfInput, TensorFlowModel
from model_compiler.models.targets.tensorrt_model import TensorRTModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_tensorrt_model() -> TensorRTModel:
    with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='y')
        weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
        output_z = tf.add(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

    compiler, config_type = compiler_repository.REPOSITORY.get(TensorFlowModel, TensorRTModel)

    return compiler(source=TensorFlowModel(inputs=[TfInput(tensor=input_x), TfInput(tensor=input_y)],
                                           outputs=[output_z],
                                           session=session),
                    config=config_type.from_json({'max_batch_size': 4}))


@pytest.mark.gpu_test
class KerasModelFileTestCase(TestCase):
    def test_get_inputs(self):
        saved_model = _make_tensorrt_model()

        self.assertEqual(saved_model.get_inputs(),
                         [ModelInput(name='x:0', data_type=tf.float32.as_datatype_enum, format=None, dims=[4]),
                          ModelInput(name='y:0', data_type=tf.float32.as_datatype_enum, format=None, dims=[4])])

    def test_get_outputs(self):
        saved_model = _make_tensorrt_model()

        self.assertEqual(saved_model.get_outputs(),
                         [ModelOutput(name='z:0', data_type=tf.float32.as_datatype_enum, dims=[4])])

    def test_save(self):
        saved_model = _make_tensorrt_model()

        with TemporaryDirectory() as save_path:
            saved_model.save(save_path)

            self.assertEqual(os.listdir(save_path), ['model.plan'])

    def test_get_platform(self):
        self.assertEqual(TensorRTModel.get_platform(), ('tensorrt_plan', tensorrt.__version__))
