# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from tempfile import TemporaryDirectory
from unittest import TestCase

import tensorrt
import tensorflow as tf
import pytest
import model_compiler.compilers.repository as compiler_repository
from model_compiler.models.sources.saved_model_file import SavedModelFile
from model_compiler.models.targets.tftrt_saved_model import TfTRTSavedModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


def _make_tftrt_saved_model(model_dir) -> TfTRTSavedModel:
    with tf.compat.v1.Session(graph=tf.Graph()) as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[None, 4], name='y')
        weight = tf.Variable(initial_value=[2.0, 3.0, 4.0, 5.0], dtype=tf.float32)
        output_z = tf.add(input_x + input_y, weight, name='z')

        session.run(weight.initializer)

        builder = tf.compat.v1.saved_model.builder.SavedModelBuilder(model_dir)
        input_x_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_x)
        input_y_tensor_info = tf.compat.v1.saved_model.build_tensor_info(input_y)
        output_tensor_info = tf.compat.v1.saved_model.build_tensor_info(output_z)

        builder.add_meta_graph_and_variables(
            sess=session,
            tags=[tf.compat.v1.saved_model.tag_constants.SERVING],
            signature_def_map={
                'serving_default': tf.compat.v1.saved_model.build_signature_def(inputs={'x': input_x_tensor_info,
                                                                                        'y': input_y_tensor_info},
                                                                                outputs={'z': output_tensor_info})
            },
            clear_devices=True
        )

        builder.save()

    compiler, config_type = compiler_repository.REPOSITORY.get(SavedModelFile, TfTRTSavedModel)

    return compiler(source=SavedModelFile(model_path=model_dir),  # pylint: disable=too-many-function-args
                    config=config_type.from_json({'max_batch_size': 4,  # pylint: disable=too-many-function-args
                                                  'input_formats': ['channels_last', 'channels_last']}))


@pytest.mark.gpu_test
class TfTRTSavedModelTestCase(TestCase):
    def test_get_inputs(self):
        with TemporaryDirectory() as model_dir:
            tftrt_saved_model = _make_tftrt_saved_model(model_dir)

            self.assertEqual(tftrt_saved_model.get_inputs(),
                             [ModelInput(name='x', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NHWC, dims=[4]),  # pylint: disable=no-member
                              ModelInput(name='y', data_type=tf.float32.as_datatype_enum,
                                         format=ModelInput.FORMAT_NHWC, dims=[4])])  # pylint: disable=no-member

    def test_get_outputs(self):
        with TemporaryDirectory() as model_dir:
            tftrt_saved_model = _make_tftrt_saved_model(model_dir)

            self.assertEqual(tftrt_saved_model.get_outputs(),
                             [ModelOutput(name='z', data_type=tf.float32.as_datatype_enum, dims=[4])])

    def test_save(self):
        with TemporaryDirectory() as model_dir:
            tftrt_saved_model = _make_tftrt_saved_model(model_dir)

            with TemporaryDirectory() as save_path:
                tftrt_saved_model.save(save_path)

                self.assertEqual(sorted(os.listdir(save_path)), ['assets', 'saved_model.pb', 'variables'])

                self.assertEqual(sorted(os.listdir(os.path.join(save_path, 'variables'))),
                                 ['variables.data-00000-of-00001', 'variables.index'])

    def test_get_platform(self):
        self.assertEqual(TfTRTSavedModel.get_platform(),
                         ('tensorflow-tensorrt', tf.version.VERSION + "-" + tensorrt.__version__))
