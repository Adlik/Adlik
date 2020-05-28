# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple, Optional, Sequence

from onnx import ModelProto

from ..data_format import DataFormat


class OnnxModel(NamedTuple):
    model_proto: ModelProto
    input_data_formats: Sequence[Optional[DataFormat]]
