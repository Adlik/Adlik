# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple, Union

import keras
import tensorflow as tf


class KerasModel(NamedTuple):
    model: Union[keras.Model, tf.keras.Model]
    session: tf.compat.v1.Session
