# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from typing import NamedTuple

import tensorflow as tf
from tensorflow import keras


class KerasModel(NamedTuple):
    model: keras.Model
    session: tf.compat.v1.Session
