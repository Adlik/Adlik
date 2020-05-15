from typing import NamedTuple, Union

import keras
import tensorflow as tf


class KerasModel(NamedTuple):
    model: Union[keras.Model, tf.keras.Model]
    session: tf.compat.v1.Session
