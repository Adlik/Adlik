import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import tensorflow as tf
import tensorflow.python.eager.context as eager_context  # pylint: disable=no-name-in-module

from model_compiler.models.targets.saved_model import Input, Output, SavedModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


class KerasModelFileTestCase(TestCase):
    def test_get_inputs(self):
        with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
            output_z = input_x + input_y + weight

        saved_model = SavedModel(inputs=[Input(name='x', tensor=input_x), Input(name='y', tensor=input_y)],
                                 outputs=[Output(name='z', tensor=output_z)],
                                 session=session)

        self.assertEqual(saved_model.get_inputs(),
                         [ModelInput(name='x', data_type=tf.float32.as_datatype_enum, format=None, dims=[4]),
                          ModelInput(name='y', data_type=tf.float32.as_datatype_enum, format=None, dims=[4])])

    def test_get_outputs(self):
        with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
            output_z = input_x + input_y + weight

        saved_model = SavedModel(inputs=[Input(name='x', tensor=input_x), Input(name='y', tensor=input_y)],
                                 outputs=[Output(name='z', tensor=output_z)],
                                 session=session)

        self.assertEqual(saved_model.get_outputs(),
                         [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[4])])

    def test_save(self):
        with eager_context.graph_mode(), tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
            input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4])
            weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
            output_z = input_x + input_y + weight

            session.run(weight.initializer)

        saved_model = SavedModel(inputs=[Input(name='x', tensor=input_x), Input(name='y', tensor=input_y)],
                                 outputs=[Output(name='z', tensor=output_z)],
                                 session=session)

        with TemporaryDirectory() as save_path:
            saved_model.save(save_path)

            self.assertEqual(sorted(os.listdir(save_path)), ['saved_model.pb', 'variables'])

            self.assertEqual(sorted(os.listdir(os.path.join(save_path, 'variables'))),
                             ['variables.data-00000-of-00001', 'variables.index'])
