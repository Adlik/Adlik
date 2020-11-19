# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple, Optional, Sequence

import tensorflow as tf

from ..data_format import DataFormat


class Input(NamedTuple):
    tensor: tf.Tensor
    data_format: Optional[DataFormat] = None


class TensorFlowModel(NamedTuple):
    inputs: Sequence[Input]
    outputs: Sequence[tf.Tensor]
    session: tf.compat.v1.Session
