# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.torch_model_file import TorchModelFile


class TorchModelFileTestCase(TestCase):
    def test_from_json_minimal(self):
        config = TorchModelFile.from_json({'input_model': 'foo'})

        self.assertEqual(config.model_path, 'foo')
        self.assertIsNone(config.script_path)

    def test_from_json_full(self):
        config = TorchModelFile.from_json({'input_model': 'foo', 'script_path': 'bar'})

        self.assertEqual(config.model_path, 'foo')
        self.assertEqual(config.script_path, 'bar')

    def test_from_env(self):
        config = TorchModelFile.from_env({'TORCH_PATH': 'model', 'SCRIPT_PATH': 'bar'})

        self.assertEqual(config.model_path, 'model')
        self.assertEqual(config.script_path, 'bar')
