# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import os
from unittest import TestCase

import tensorflow as tf

import model_compiler.compilers.openvino_model_file_to_openvino_model as compiler
from model_compiler.compilers.openvino_model_file_to_openvino_model import Config
from model_compiler.models.sources.openvino_model_file import OpenvinoModelFile
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'enable_int8': True,
                                           'max_batch_size': 1,
                                           'quantization_json_path': 'path1',
                                           'checker_yaml_path': 'path2',
                                           'data_source_path': 'path3',
                                           'annotation_file_path': 'path4',
                                           'evaluate': True}),
                         Config(enable_int8=True,
                                max_batch_size=1,
                                quantization_json_path='path1',
                                checker_yaml_path='path2',
                                data_source_path='path3',
                                annotation_file_path='path4',
                                evaluate=True))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'ENABLE_INT8': '1',
                                          'MAX_BATCH_SIZE': '1',
                                          'QUANTIZATION_JSON_PATH': 'path1',
                                          'CHECKER_YAML_PATH': 'path2',
                                          'DATA_SOURCE_PATH': 'path3',
                                          'ANNOTATION_FILE_PATH': 'path4',
                                          'EVALUATE': '1'}),
                         Config(enable_int8=True,
                                max_batch_size=1,
                                quantization_json_path='path1',
                                checker_yaml_path='path2',
                                data_source_path='path3',
                                annotation_file_path='path4',
                                evaluate=True))


class CompileSourceTestCase(TestCase):
    def test_compile_with_enable_int8_default_engine(self):
        data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../data/openvino/pot')
        config = Config.from_json({'enable_int8': True,
                                   'max_batch_size': 1,
                                   'quantization_json_path': os.path.join(data_path,
                                                                          'mobilenet_v2_pytorch_default.json'),
                                   'checker_yaml_path': os.path.join(data_path, 'mobilenet_v2_pytorch.yaml'),
                                   'data_source_path': os.path.join(data_path, 'imagenet'),
                                   'annotation_file_path': os.path.join(data_path, 'val.txt'),
                                   'evaluate': False})

        compiled = compiler.compile_source(OpenvinoModelFile(model_path=os.path.join(data_path, 'fp32')), config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='data', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[3, 224, 224])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='prob', data_type=tf.float32.as_datatype_enum,
                                      dims=[1000])])  # pylint: disable=no-member

    def test_compile_with_enable_int8_simplified_engine(self):
        data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../data/openvino/pot')
        config = Config.from_json({'enable_int8': True,
                                   'max_batch_size': 1,
                                   'quantization_json_path': os.path.join(data_path,
                                                                          'mobilenet_v2_pytorch_simplified.json'),
                                   'checker_yaml_path': None,
                                   'data_source_path': os.path.join(data_path, 'imagenet'),
                                   'annotation_file_path': None,
                                   'evaluate': False})

        compiled = compiler.compile_source(OpenvinoModelFile(model_path=os.path.join(data_path, 'fp32')), config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='data', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[3, 224, 224])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='prob', data_type=tf.float32.as_datatype_enum,
                                      dims=[1000])])  # pylint: disable=no-member

    def test_compile_with_unable_int8(self):
        config = Config.from_json({'enable_int8': False, 'max_batch_size': 1})
        data_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), '../data/openvino/pot')
        compiled = compiler.compile_source(OpenvinoModelFile(model_path=os.path.join(data_path, 'fp32')), config)
        self.assertEqual(compiled.get_inputs(),
                         [ModelInput(name='data', data_type=tf.float32.as_datatype_enum,
                                     format=ModelInput.FORMAT_NONE, dims=[3, 224, 224])])  # pylint: disable=no-member
        self.assertEqual(compiled.get_outputs(),
                         [ModelOutput(name='prob', data_type=tf.float32.as_datatype_enum,
                                      dims=[1000])])  # pylint: disable=no-member
