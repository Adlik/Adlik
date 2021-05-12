# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.mxnet_model_file import MxnetModelFile


class ONNXModelFileTestCase(TestCase):
    def test_from_json_minimal(self):
        config = MxnetModelFile.from_json({'input_model': 'foo'})

        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = MxnetModelFile.from_env({'MXNET_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')
