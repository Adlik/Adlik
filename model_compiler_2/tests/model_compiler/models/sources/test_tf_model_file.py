# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.tf_model_file import TfModelFile


class TfModelFileTestCase(TestCase):
    def test_from_json(self):
        config = TfModelFile.from_json({'input_model': 'foo'})
        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = TfModelFile.from_env({'CHECKPOINT_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')
