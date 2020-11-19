# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from ..models.irs.tf_frozen_graph_model import Input, TensorFlowFrozenGraphModel
from ..models.irs.tf_model import TensorFlowModel


@repository.REPOSITORY.register(source_type=TensorFlowModel, target_type=TensorFlowFrozenGraphModel)
def compile_source(source: TensorFlowModel) -> TensorFlowFrozenGraphModel:
    graph_def = tf.compat.v1.graph_util.convert_variables_to_constants(
        sess=source.session,
        input_graph_def=source.session.graph_def,
        output_node_names=[source_output.op.name for source_output in source.outputs]
    )

    graph_def = tf.compat.v1.graph_util.remove_training_nodes(input_graph=graph_def)

    return TensorFlowFrozenGraphModel(graph_def=graph_def,
                                      inputs=[Input(name=source_input.tensor.name, data_format=source_input.data_format)
                                              for source_input in source.inputs],
                                      outputs=[source_output.name for source_output in source.outputs])
