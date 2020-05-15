from typing import NamedTuple, Optional, Sequence

import tensorflow as tf

from ..data_format import DataFormat


class Input(NamedTuple):
    name: str
    data_format: Optional[DataFormat] = None


class TensorFlowFrozenGraphModel(NamedTuple):
    graph_def: tf.compat.v1.GraphDef
    inputs: Sequence[Input]
    outputs: Sequence[str]
