# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from model_compiler.models import data_format
from model_compiler.models.data_format import DataFormat
from model_compiler.protos.generated.model_config_pb2 import ModelInput


class AsModelConfigDataFormatTestCase(TestCase):
    def test_none(self):
        self.assertEqual(data_format.as_model_config_data_format(None),
                         ModelInput.FORMAT_NONE)  # pylint: disable=no-member

    def test_channels_first(self):
        self.assertEqual(data_format.as_model_config_data_format(DataFormat.CHANNELS_FIRST),
                         ModelInput.FORMAT_NCHW)  # pylint: disable=no-member

    def test_channels_last(self):
        self.assertEqual(data_format.as_model_config_data_format(DataFormat.CHANNELS_LAST),
                         ModelInput.FORMAT_NHWC)  # pylint: disable=no-member
