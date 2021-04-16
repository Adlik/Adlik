# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from .. import utilities
from ..models.irs.tf_model import Input, TensorFlowModel
from ..models.sources.tf_model_file import TfModelFile
from ..tensorflow_util import Config, get_inputs


def _load_model(session, model_path, config):
    tf_saver = tf.compat.v1.train.import_meta_graph(model_path + '.meta', clear_devices=True)
    tf_saver.restore(sess=session, save_path=model_path)

    inputs = [Input(name, data_format) for name, data_format in get_inputs(session.graph, config)]
    outputs = config.get_output_tensors_from_graph(session.graph)

    return inputs, outputs


@repository.REPOSITORY.register(source_type=TfModelFile, target_type=TensorFlowModel, config_type=Config)
def compile_source(source: TfModelFile, config: Config) -> TensorFlowModel:
    with tf.Graph().as_default(), \
         tf.compat.v1.Session(config=utilities.get_tf_cpu_only_config()).as_default() as session:
        inputs, outputs = _load_model(session, source.model_path, config)

    utilities.judge_batch_size([model_input.tensor.shape for model_input in inputs],
                               [model_output.shape for model_output in outputs])

    return TensorFlowModel(inputs, outputs, session)
