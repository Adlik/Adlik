# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf
from onnx import defs
from tf2onnx import optimizer, tfonnx, tf_loader

from . import repository
from ..models.irs.onnx_model import OnnxModel
from ..models.sources.tf_model_file import TfModelFile
from ..tensorflow_util import Config


@repository.REPOSITORY.register(source_type=TfModelFile, target_type=OnnxModel, config_type=Config)
def compile_source(source: TfModelFile, config: Config) -> OnnxModel:
    graph_def, inputs, outputs = tf_loader.from_checkpoint(source.model_path + '.meta',
                                                           config.input_names, config.output_names)

    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def=graph_def, name='')

    onnx_graph = tfonnx.process_tf_graph(tf_graph=graph,
                                         opset=min(10, defs.onnx_opset_version()),
                                         input_names=inputs,
                                         output_names=outputs)

    onnx_graph = optimizer.optimize_graph(graph=onnx_graph)
    model_proto = onnx_graph.make_model('ONNX model generated from TensorFlow model.')

    input_name_to_index = {source_input: i for i, source_input in enumerate(inputs)}

    sorted_graph_inputs = sorted(model_proto.graph.input,
                                 key=lambda target_input: input_name_to_index[target_input.name])

    del model_proto.graph.input[:]
    model_proto.graph.input.extend(sorted_graph_inputs)

    return OnnxModel(model_proto=model_proto,
                     input_data_formats=config.data_formats)
