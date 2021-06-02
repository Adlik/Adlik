# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

# pylint: disable=no-member

import enum
from enum import Enum
from onnx import TensorProto as OnnxTensorProto
from tensorflow.core.framework.types_pb2 import DataType as TfDataType  # pylint: disable=no-name-in-module
import torch
