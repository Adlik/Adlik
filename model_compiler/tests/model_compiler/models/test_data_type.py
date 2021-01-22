# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase

from onnx import TensorProto as OnnxTensorProto
from tensorflow.core.framework.types_pb2 import DataType as TfDataType  # pylint: disable=no-name-in-module
from tensorrt import DataType as TrtDataType

from model_compiler.models.data_type import DataType

_ONNX_DATA_TYPE = OnnxTensorProto.DataType  # pylint: disable=no-member


class DataTypeTestCase(TestCase):
    def test_from_tf_data_type(self):
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_BOOL), DataType.BOOL)

        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_INT8), DataType.INT8)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_UINT8), DataType.UINT8)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_INT16), DataType.INT16)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_UINT16), DataType.UINT16)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_INT32), DataType.INT32)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_UINT32), DataType.UINT32)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_INT64), DataType.INT64)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_UINT64), DataType.UINT64)

        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_HALF), DataType.FLOAT16)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_BFLOAT16), DataType.BFLOAT16)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_FLOAT), DataType.FLOAT)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_DOUBLE), DataType.DOUBLE)

        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_COMPLEX64), DataType.COMPLEX64)
        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_COMPLEX128), DataType.COMPLEX128)

        self.assertEqual(DataType.from_tf_data_type(TfDataType.DT_STRING), DataType.STRING)

    def test_to_tf_data_type(self):
        self.assertEqual(DataType.BOOL.to_tf_data_type(), TfDataType.DT_BOOL)

        self.assertEqual(DataType.INT8.to_tf_data_type(), TfDataType.DT_INT8)
        self.assertEqual(DataType.UINT8.to_tf_data_type(), TfDataType.DT_UINT8)
        self.assertEqual(DataType.INT16.to_tf_data_type(), TfDataType.DT_INT16)
        self.assertEqual(DataType.UINT16.to_tf_data_type(), TfDataType.DT_UINT16)
        self.assertEqual(DataType.INT32.to_tf_data_type(), TfDataType.DT_INT32)
        self.assertEqual(DataType.UINT32.to_tf_data_type(), TfDataType.DT_UINT32)
        self.assertEqual(DataType.INT64.to_tf_data_type(), TfDataType.DT_INT64)
        self.assertEqual(DataType.UINT64.to_tf_data_type(), TfDataType.DT_UINT64)

        self.assertEqual(DataType.FLOAT16.to_tf_data_type(), TfDataType.DT_HALF)
        self.assertEqual(DataType.BFLOAT16.to_tf_data_type(), TfDataType.DT_BFLOAT16)
        self.assertEqual(DataType.FLOAT.to_tf_data_type(), TfDataType.DT_FLOAT)
        self.assertEqual(DataType.DOUBLE.to_tf_data_type(), TfDataType.DT_DOUBLE)

        self.assertEqual(DataType.COMPLEX64.to_tf_data_type(), TfDataType.DT_COMPLEX64)
        self.assertEqual(DataType.COMPLEX128.to_tf_data_type(), TfDataType.DT_COMPLEX128)

        self.assertEqual(DataType.STRING.to_tf_data_type(), TfDataType.DT_STRING)

    def test_from_onnx_data_type(self):
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.BOOL), DataType.BOOL)

        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.INT8), DataType.INT8)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.UINT8), DataType.UINT8)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.INT16), DataType.INT16)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.UINT16), DataType.UINT16)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.INT32), DataType.INT32)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.UINT32), DataType.UINT32)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.INT64), DataType.INT64)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.UINT64), DataType.UINT64)

        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.FLOAT16), DataType.FLOAT16)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.BFLOAT16), DataType.BFLOAT16)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.FLOAT), DataType.FLOAT)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.DOUBLE), DataType.DOUBLE)

        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.COMPLEX64), DataType.COMPLEX64)
        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.COMPLEX128), DataType.COMPLEX128)

        self.assertEqual(DataType.from_onnx_data_type(_ONNX_DATA_TYPE.STRING), DataType.STRING)

    def test_to_onnx_data_type(self):
        self.assertEqual(DataType.BOOL.to_onnx_data_type(), _ONNX_DATA_TYPE.BOOL)

        self.assertEqual(DataType.INT8.to_onnx_data_type(), _ONNX_DATA_TYPE.INT8)
        self.assertEqual(DataType.UINT8.to_onnx_data_type(), _ONNX_DATA_TYPE.UINT8)
        self.assertEqual(DataType.INT16.to_onnx_data_type(), _ONNX_DATA_TYPE.INT16)
        self.assertEqual(DataType.UINT16.to_onnx_data_type(), _ONNX_DATA_TYPE.UINT16)
        self.assertEqual(DataType.INT32.to_onnx_data_type(), _ONNX_DATA_TYPE.INT32)
        self.assertEqual(DataType.UINT32.to_onnx_data_type(), _ONNX_DATA_TYPE.UINT32)
        self.assertEqual(DataType.INT64.to_onnx_data_type(), _ONNX_DATA_TYPE.INT64)
        self.assertEqual(DataType.UINT64.to_onnx_data_type(), _ONNX_DATA_TYPE.UINT64)

        self.assertEqual(DataType.FLOAT16.to_onnx_data_type(), _ONNX_DATA_TYPE.FLOAT16)
        self.assertEqual(DataType.BFLOAT16.to_onnx_data_type(), _ONNX_DATA_TYPE.BFLOAT16)
        self.assertEqual(DataType.FLOAT.to_onnx_data_type(), _ONNX_DATA_TYPE.FLOAT)
        self.assertEqual(DataType.DOUBLE.to_onnx_data_type(), _ONNX_DATA_TYPE.DOUBLE)

        self.assertEqual(DataType.COMPLEX64.to_onnx_data_type(), _ONNX_DATA_TYPE.COMPLEX64)
        self.assertEqual(DataType.COMPLEX128.to_onnx_data_type(), _ONNX_DATA_TYPE.COMPLEX128)

        self.assertEqual(DataType.STRING.to_onnx_data_type(), _ONNX_DATA_TYPE.STRING)

    def test_from_tensorrt_data_type(self):
        self.assertEqual(DataType.from_tensorrt_data_type(TrtDataType.BOOL), DataType.BOOL)

        self.assertEqual(DataType.from_tensorrt_data_type(TrtDataType.INT8), DataType.INT8)
        self.assertEqual(DataType.from_tensorrt_data_type(TrtDataType.INT32), DataType.INT32)

        self.assertEqual(DataType.from_tensorrt_data_type(TrtDataType.HALF), DataType.FLOAT16)
        self.assertEqual(DataType.from_tensorrt_data_type(TrtDataType.FLOAT), DataType.FLOAT)

    def test_to_tensorrt_data_type(self):
        self.assertEqual(DataType.BOOL.to_tensorrt_data_type(), TrtDataType.BOOL)

        self.assertEqual(DataType.INT8.to_tensorrt_data_type(), TrtDataType.INT8)
        self.assertEqual(DataType.INT32.to_tensorrt_data_type(), TrtDataType.INT32)

        self.assertEqual(DataType.FLOAT16.to_tensorrt_data_type(), TrtDataType.HALF)
        self.assertEqual(DataType.FLOAT.to_tensorrt_data_type(), TrtDataType.FLOAT)
