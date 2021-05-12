# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.caffe_model_file import CaffeModelFile


class CaffeModelFileTestCase(TestCase):
    def test_from_json_minimal(self):
        config = CaffeModelFile.from_json({'input_model': 'foo'})

        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = CaffeModelFile.from_env({'CAFFE_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')
