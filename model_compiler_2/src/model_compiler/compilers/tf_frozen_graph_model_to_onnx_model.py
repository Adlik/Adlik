# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf
from onnx import defs
from tf2onnx import tfonnx

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.irs.tf_frozen_graph_model import TensorFlowFrozenGraphModel


@repository.REPOSITORY.register(source_type=TensorFlowFrozenGraphModel, target_type=OnnxModel)
def compile_source(source: TensorFlowFrozenGraphModel) -> OnnxModel:
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def=source.graph_def, name='')

    onnx_graph = tfonnx.process_tf_graph(tf_graph=graph,
                                         opset=defs.onnx_opset_version(),
                                         input_names=[source_input.name for source_input in source.inputs],
                                         output_names=list(source.outputs))

    model_proto = onnx_graph.make_model("ONNX model generated from TensorFlow frozen graph model.")

    input_name_to_index = {source_input.name: i for i, source_input in enumerate(source.inputs)}

    sorted_graph_inputs = sorted(model_proto.graph.input,
                                 key=lambda target_input: input_name_to_index[target_input.name])

    del model_proto.graph.input[:]
    model_proto.graph.input.extend(sorted_graph_inputs)

    return OnnxModel(model_proto=model_proto,
                     input_data_formats=[source_input.data_format for source_input in source.inputs])
