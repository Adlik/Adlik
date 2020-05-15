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

    return OnnxModel(model_proto=model_proto,
                     input_data_formats={source_input.name: source_input.data_format for source_input in source.inputs})
