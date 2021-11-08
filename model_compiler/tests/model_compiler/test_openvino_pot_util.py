# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler import openvino_pot_util
from model_compiler.openvino_pot_util import Config


class OpenVinoPotUtilTestCase(TestCase):

    def test_get_pot_params(self):
        config = Config(enable_int8=True,
                        max_batch_size=1,
                        quantization_json_path='path1',
                        checker_yaml_path='path2',
                        data_source_path='path3',
                        annotation_file_path='path4',
                        evaluate=True)
        params = openvino_pot_util.get_pot_params('output_path', 'test.json', config)
        self.assertTrue('evaluate' in params.keys())
        self.assertEqual(params['evaluate'], None)
