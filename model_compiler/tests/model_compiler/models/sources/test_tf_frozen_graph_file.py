# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models.sources.tf_frozen_graph_file import FrozenGraphFile


class FrozenGraphFileTestCase(TestCase):
    def test_from_json(self):
        config = FrozenGraphFile.from_json({'input_model': 'foo'})
        self.assertEqual(config.model_path, 'foo')

    def test_from_env(self):
        config = FrozenGraphFile.from_env({'FROZEN_GRAPH_PATH': 'model'})
        self.assertEqual(config.model_path, 'model')
