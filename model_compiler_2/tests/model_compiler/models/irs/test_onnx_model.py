# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from onnx import GraphProto, ModelProto, TensorProto, ValueInfoProto

from model_compiler.models.irs.onnx_model import OnnxModel


class OnnxModelTestCase(TestCase):
    def test_get_inputs(self):
        model = OnnxModel(model_proto=ModelProto(graph=GraphProto(initializer=[TensorProto(name='y')],
                                                                  input=[ValueInfoProto(name='x'),
                                                                         ValueInfoProto(name='y'),
                                                                         ValueInfoProto(name='z')])),
                          input_data_formats=[None, None])

        self.assertEqual(model.get_inputs(), [ValueInfoProto(name='x'), ValueInfoProto(name='z')])
