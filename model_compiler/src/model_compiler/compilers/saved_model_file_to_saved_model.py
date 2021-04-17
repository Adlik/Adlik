# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorflow as tf

from . import repository
from .. import utilities
from ..models.sources.saved_model_file import SavedModelFile
from ..models.targets.saved_model import SavedModel, Input, Output
from ..tensorflow_util import Config, get_inputs


@repository.REPOSITORY.register(source_type=SavedModelFile, target_type=SavedModel, config_type=Config)
def compile_source(source: SavedModelFile, config: Config) -> SavedModel:
    with tf.Graph().as_default(), \
         tf.compat.v1.Session(config=utilities.get_tf_cpu_only_config()).as_default() as sess:
        tf.compat.v1.saved_model.loader.load(sess,
                                             [tf.compat.v1.saved_model.tag_constants.SERVING],
                                             source.model_path)
        graph = tf.compat.v1.get_default_graph()
        inputs = [Input(tensor.name, tensor, data_format) for tensor, data_format in get_inputs(graph, config)]
        outputs = [Output(tensor.name, tensor) for tensor in config.get_output_tensors_from_graph(graph)]
        return SavedModel(inputs=inputs, outputs=outputs, session=sess)
