# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase
import torch
from model_compiler.torchscript_util import Config
from model_compiler.models.data_format import DataFormat


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(Config.from_json({'input_names': ['input'],
                                           'input_shapes': [1, 10],
                                           'data_type': 'FLOAT',
                                           'max_batch_size': 1,
                                           'input_formats': ['channels_first']}),
                         Config(input_names=['input'],
                                input_shapes=[[1, 10]],
                                data_type=torch.float,     # pylint: disable=no-member
                                max_batch_size=1,
                                input_formats=[DataFormat.CHANNELS_FIRST],
                                ))

    def test_from_env(self):
        self.assertEqual(Config.from_env({'INPUT_NAMES': 'input',
                                          'INPUT_SHAPES': '[1, 2, 3, 4]',
                                          'MAX_BATCH_SIZE': '1',
                                          'DATA_TYPE': 'FLOAT',
                                          'INPUT_FORMATS': 'channels_last'
                                          }),
                         Config(input_names=['input'],
                                input_shapes=[[1, 2, 3, 4]],
                                max_batch_size=1,
                                data_type=torch.float,   # pylint: disable=no-member
                                input_formats=[DataFormat.CHANNELS_LAST]))
