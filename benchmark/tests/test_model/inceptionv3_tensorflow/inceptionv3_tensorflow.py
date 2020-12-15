# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
InceptionV3 model
"""

import tensorflow as tf
from tensorflow.python.framework.convert_to_constants import \
    convert_variables_to_constants_v2  # pylint: disable=no-name-in-module
import os


def _get_model():
    return tf.keras.applications.inception_v3.InceptionV3(include_top=True, weights='imagenet',
                                                          input_tensor=None, input_shape=None,
                                                          pooling=None,
                                                          classes=1000)


def main():
    model = _get_model()
    print(model.output_names)
    save_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "model_pb")
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    # saved model to SavedModel format
    tf.saved_model.save(model, save_path)

    # Convert Keras model to ConcreteFunction
    full_model = tf.function(lambda x: model(x))
    full_model = full_model.get_concrete_function(
        tf.TensorSpec(model.inputs[0].shape, model.inputs[0].dtype))

    # Get frozen ConcreteFunction
    frozen_func = convert_variables_to_constants_v2(full_model)
    frozen_func.graph.as_graph_def()

    layers = [op.name for op in frozen_func.graph.get_operations()]
    print("-" * 50)
    print("Frozen model layers: ")
    for layer in layers:
        print(layer)

    print("-" * 50)
    print("Frozen model inputs: ")
    print(frozen_func.inputs)
    print("Frozen model outputs: ")
    print(frozen_func.outputs)

    # Save frozen graph from frozen ConcreteFunction to hard drive
    tf.io.write_graph(graph_or_graph_def=frozen_func.graph,
                      logdir="./frozen_models",
                      name=os.path.join(save_path, "frozen_graph.pb"),
                      as_text=False)


if __name__ == '__main__':
    main()
