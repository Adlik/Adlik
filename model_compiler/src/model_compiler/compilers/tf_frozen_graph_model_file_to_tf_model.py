# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from .. import utilities
from ..models.irs.tf_model import Input, TensorFlowModel
from ..models.sources.tf_frozen_graph_file import FrozenGraphFile
from ..tensorflow_util import Config, get_inputs


@repository.REPOSITORY.register(source_type=FrozenGraphFile, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: FrozenGraphFile, config: Config) -> TensorFlowModel:
    graph_def = tf.compat.v1.GraphDef()

    with open(source.model_path, 'rb') as graph_file:
        graph_def.ParseFromString(graph_file.read())

    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name='')
    inputs = [Input(name, data_format) for name, data_format in get_inputs(graph, config)]
    outputs = config.get_output_tensors_from_graph(graph)
    utilities.judge_batch_size([model_input[0].shape for model_input in inputs],
                               [model_output.shape for model_output in outputs])

    return TensorFlowModel(inputs=inputs,
                           outputs=outputs,
                           session=tf.compat.v1.Session(graph=graph, config=utilities.get_tf_cpu_only_config()))
