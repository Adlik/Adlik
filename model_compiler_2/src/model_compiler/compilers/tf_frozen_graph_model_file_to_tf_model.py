# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf
from . import repository
from .tf_model_file_to_tf_model import Config, _get_tensor_by_fuzzy_name
from ..models.irs.tf_model import TensorFlowModel, Input
from ..models.sources.tf_frozen_graph_file import FrozenGraphFile


def _load_model(model_path, config):
    with tf.io.gfile.GFile(model_path, "rb") as graph_file:
        graph_def = tf.compat.v1.GraphDef()
        graph_def.ParseFromString(graph_file.read())
        tf.import_graph_def(graph_def, name="")
        graph = tf.compat.v1.get_default_graph()
        return _get_inputs(graph, config), _get_outputs(graph, config)


def _get_inputs(graph, config):
    try:
        return [Input(_get_tensor_by_fuzzy_name(graph, name), data_format) for name, data_format in
                config.input_info]
    except KeyError:
        ops = graph.get_operations()
        input_objs = []
        for operation in ops:
            if not operation.inputs and operation.type == 'Placeholder':
                input_objs.append(Input(operation.outputs[0], None))
        return input_objs


def _get_outputs(graph, config):
    try:
        return [_get_tensor_by_fuzzy_name(graph, name) for name in config.output_names]
    except KeyError:
        ops = graph.get_operations()
        outputs_set = set(ops)
        for operation in ops:
            for input_tensor in operation.inputs:
                if input_tensor.op in outputs_set:
                    outputs_set.remove(input_tensor.op)
        output_ops = [op for op in list(outputs_set) if
                      op.type not in ['Assign', 'NoOp', 'SaveV2', 'IsVariableInitialized',
                                      'Placeholder', 'Const']]
        return [op.outputs[0] for op in output_ops]


@repository.REPOSITORY.register(source_type=FrozenGraphFile, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: FrozenGraphFile, config: Config) -> TensorFlowModel:
    with tf.Graph().as_default(), tf.compat.v1.Session() as sess:
        inputs, outputs = _load_model(source.model_path, config)
    return TensorFlowModel(inputs, outputs, sess)
