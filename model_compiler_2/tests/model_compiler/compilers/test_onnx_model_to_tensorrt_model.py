# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

import pytest
import tensorflow as tf
import tensorflow.python.eager.context as eager_context
from tensorrt import ICudaEngine

import model_compiler.compilers.onnx_model_to_tensorrt_model as compiler
import model_compiler.compilers.tf_frozen_graph_model_to_onnx_model as onnx_compiler
import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as frozen_graph_compiler
from model_compiler.compilers.onnx_model_to_tensorrt_model import Config
from model_compiler.models.data_type import DataType
from model_compiler.models.irs.tf_model import Input as TfInput, TensorFlowModel
from model_compiler.models.targets.tensorrt_model import Input, Output


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'max_batch_size': 7}),
                         Config(max_batch_size=7))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'max_batch_size': '7'}),
                         Config(max_batch_size=7))


def _make_onnx_model(func):
    with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='y')
        output_z = func(input_x, input_y, session)

    frozen_graph_model = frozen_graph_compiler.compile_source(
        source=TensorFlowModel(inputs=[TfInput(tensor=input_x), TfInput(tensor=input_y)],
                               outputs=[output_z],
                               session=session)
    )

    return onnx_compiler.compile_source(frozen_graph_model)


@pytest.mark.gpu_test
class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        onnx_model = _make_onnx_model(lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'))
        compiled = compiler.compile_source(source=onnx_model, config=Config(max_batch_size=4))

        self.assertEqual(compiled.inputs,
                         [Input(name='x:0', data_type=DataType.FLOAT, shape=[4], data_format=None),
                          Input(name='y:0', data_type=DataType.FLOAT, shape=[4], data_format=None)])

        self.assertEqual(compiled.outputs, [Output(name='z:0', data_type=DataType.FLOAT, shape=[4])])

        self.assertIsInstance(compiled.cuda_engine, ICudaEngine)
