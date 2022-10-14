# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import NamedTemporaryFile
from tempfile import TemporaryDirectory

from unittest import TestCase

import tensorflow as tf
import onnx
import onnx.utils

import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as tf_model_compiler
import model_compiler.compilers.tf_frozen_graph_model_to_onnx_model as frozen_graph_compiler
import model_compiler.compilers.onnx_model_file_to_enflame_model as enflame_model_compiler
from model_compiler.compilers.onnx_model_file_to_enflame_model import Config

from model_compiler.models.targets.enflame_model import EnflameModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput
from model_compiler.models.sources.onnx_model_file import ONNXModelFile
from model_compiler.models.irs.tf_model import Input, TensorFlowModel

def _make_onnx_model():
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[3, 4], name='x')
        weight = tf.Variable(initial_value=4.2, dtype=tf.float32)
        output_z = tf.multiply(input_x , weight, name='z')

        session.run(weight.initializer)

    frozen_graph_model = tf_model_compiler.compile_source(
        source=TensorFlowModel(inputs=[Input(tensor=input_x)],
                               outputs=[output_z],
                               session=session)
    )
    return frozen_graph_compiler.compile_source(frozen_graph_model)


def _make_enflame_model() -> EnflameModel:
    with NamedTemporaryFile(suffix='.onnx') as model_file:
            onnx.save_model(_make_onnx_model().model_proto, model_file.name)

            config = Config.from_json({'input_formats': ['channels_first']})
            compiled = enflame_model_compiler.compile_source(source=ONNXModelFile(model_file.name), config=config)
            return compiled

class EnflameModelFileTestCase(TestCase):
    def test_get_inputs(self):
        engine = _make_enflame_model()

        self.assertEqual(engine.get_inputs(), [ModelInput(name='x:0', data_type='DT_FLOAT',
                                                          format='FORMAT_NCHW', dims=[4])])

    def test_get_outputs(self):
        engine = _make_enflame_model()

        self.assertEqual(engine.get_outputs(), [ModelOutput(name='z:0', data_type='DT_FLOAT', dims=[4])])

    def test_save(self):
        engine = _make_enflame_model()

        with TemporaryDirectory() as save_path:
            engine.save(save_path)

            self.assertEqual(os.listdir(save_path), ['model.exec'])

    def test_get_platform(self):
        self.assertEqual(EnflameModel.get_platform(), ('tops-inference','2.0.96'))

