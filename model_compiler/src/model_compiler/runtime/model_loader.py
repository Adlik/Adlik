# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Model loader, loader model into graph
"""

from abc import abstractmethod
import os
import sys

import keras
import keras.backend as K
from keras.layers import Layer
import onnx
import onnx.utils
import tensorflow as tf

from .importer import import_file
from ..log_util import get_logger

_LOGGER = get_logger(__name__)


def _get_tensor_shape_in_list(tensor):
    """
    Get tensor shape in format
    :param tensor: Tensor
    :return:list, not contain batch dimension
    """
    shape = tensor.shape.as_list()
    if shape[0] is None:
        shape.pop(0)
    shape = [-1 if i is None else i for i in shape]
    return shape


class _Input:
    """
    Input description, for export config file
    """

    def __init__(self, tensor, data_format):
        self.tensor = tensor
        self.name = tensor.op.name
        self.shape = _get_tensor_shape_in_list(tensor)
        self.dtype = tensor.dtype
        self.data_format = data_format

    def __str__(self):
        return '(Input name: %s, shape: %s, dtype: %s, data_format: %s)' % (
            self.name, self.shape, self.dtype, self.data_format)


class _Output:
    """
    Output description, for export config file
    """

    def __init__(self, tensor):
        self.tensor = tensor
        self.name = tensor.op.name
        self.shape = _get_tensor_shape_in_list(tensor)
        self.dtype = tensor.dtype

    def __str__(self):
        return '(Output name: %s, shape: %s, dtype: %s)' % (
            self.name, self.shape, self.dtype)


class _Loader:
    """
    Model loader base, load model to graph
    """

    def __init__(self):
        self.custom_object = None

    def load_model(self, model_path, script_path, callback):
        """
        Load model
        :param model_path: model path, can be *.h5, *.pb, *.ckpt
        :param script_path: model.py
        :param callback: callback after load model to graph
        :return:
        """
        _LOGGER.info('Begin to load model, data format: %s', K.image_data_format())
        config = tf.compat.v1.ConfigProto(
            gpu_options=tf.compat.v1.GPUOptions(per_process_gpu_memory_fraction=0.1, allow_growth=True),
            allow_soft_placement=True)
        with tf.compat.v1.Session(graph=tf.Graph(), config=config) as session:
            K.set_learning_phase(0)
            self.custom_object = self._parse_custom_objects(script_path)
            inputs, outputs = self._do_load(session, model_path, script_path)
            callback(session, inputs, outputs)
            return self.custom_object, inputs, outputs

    @abstractmethod
    def _do_load(self, session, model_path, script_path):
        pass

    @staticmethod
    def _parse_custom_objects(script_path):
        """
        parse custom layer object
        :param script_path:
        :return:
        """
        _LOGGER.info('Begin to parse custom object, data format: %s', K.image_data_format())

        if not os.path.exists(script_path):
            _LOGGER.warning('Script path %s not exist', script_path)
            return None

        module_name = os.path.basename(script_path)
        try:
            keras_model = import_file(script_path)
            custom_dict = {}
            for item in dir(keras_model):
                item_obj = getattr(keras_model, item)
                # pylint: disable=unidiomatic-typecheck
                if type(item_obj) == type(Layer) \
                        and issubclass(item_obj, Layer) \
                        and not item_obj.__module__.startswith('tensorflow') \
                        and not item_obj.__module__.startswith('keras'):
                    custom_dict[item] = item_obj
            return custom_dict
        except Exception as error:  # pylint: disable=broad-except
            _LOGGER.error('_parse_custom_objects exception, error: %s', error)
            return None
        finally:
            if module_name in sys.modules:
                del sys.modules[module_name]


class KerasModelLoader(_Loader):
    """
    Load keras model
    """

    def __init__(self, input_layer_names, output_layer_names):
        super(KerasModelLoader, self).__init__()
        self.input_layer_names = input_layer_names
        self.output_layer_names = output_layer_names

    def _do_load(self, session, model_path, _):
        _LOGGER.info('Begin to load h5 model, data format: %s, model_path: %s', K.image_data_format(), model_path)
        model = keras.models.load_model(str(model_path), custom_objects=self.custom_object, compile=False)
        _LOGGER.info('After load h5 model, model: %s', model)
        return self._update_tensors(model)

    def _update_tensors(self, model):
        return self._update_inputs(model), self._update_outputs(model)

    def _update_inputs(self, model):
        inputs = []
        for layer_name in self.input_layer_names:
            layer = self._get_layer(model, layer_name)
            tensors = layer.input
            if not isinstance(tensors, list):
                tensors = [tensors]
            data_format = None
            try:
                data_format = layer.data_format
            except AttributeError:
                try:
                    if isinstance(layer, keras.layers.InputLayer):
                        # pylint:disable=protected-access
                        next_layer = layer._outbound_nodes[0].outbound_layer
                        data_format = next_layer.data_format
                except Exception as error:  # pylint: disable=broad-except
                    _LOGGER.info('Not found layer %s data format, error: %s', layer_name, error)
            inputs.extend([_Input(tensor, data_format) for tensor in tensors])
        _LOGGER.info('Update model input info: %s', inputs)
        for i in inputs:
            _LOGGER.info('dump input: %s', i)
        return inputs

    def _update_outputs(self, model):
        outputs = []
        for layer_name in self.output_layer_names:
            tensors = self._get_layer(model, layer_name).output
            if not isinstance(tensors, list):
                tensors = [tensors]
            outputs.extend([_Output(tensor) for tensor in tensors])
        _LOGGER.info('Update model output info: %s', outputs)
        for output in outputs:
            _LOGGER.info('dump output: %s', output)
        return outputs

    @staticmethod
    def _get_layer(model, layer_name):
        """
        Get layer by layer_name
        :param model:
        :param layer_name: layer name, split by /
        :return:
        """
        if not layer_name or not model:
            return None
        layer_names = layer_name.split('/')
        search_layer = None
        for single_layer_name in layer_names:
            find = False
            for layer in model.layers:
                if layer.name == single_layer_name:
                    find = True
                    search_layer = layer
                    break
            if not find:
                return None
        return search_layer


class CheckpointLoader(_Loader):
    """
    Load tensorflow checkpoint
    """

    def __init__(self, input_names, output_names, input_formats):
        _LOGGER.info('Construct checkpoint loader, inputs: %s, outputs: %s, input formats: %s', input_names,
                     output_names, input_formats)
        super(CheckpointLoader, self).__init__()
        self.input_names = input_names
        self.output_names = output_names
        self.input_formats = input_formats
        if not isinstance(self.input_formats, list):
            self.input_formats = [self.input_formats]
        if len(self.input_formats) < len(self.input_names):
            self.input_formats.extend([None for _ in range(len(self.input_formats) - len(self.input_formats))])

    def _do_load(self, session, model_path, script_path):
        _LOGGER.info('Begin to load checkpoint, data format: %s, checkpoint path: %s', K.image_data_format(),
                     model_path)

        meta_path = model_path + '.meta'
        if os.path.exists(meta_path):
            _LOGGER.info('Import meta from .meta file')
            tf_saver = tf.train.import_meta_graph(model_path + '.meta', clear_devices=True, graph=session.graph)
        elif os.path.exists(script_path):
            _LOGGER.info('Import meta from py script')
            module = import_file(script_path)
            if hasattr(module, "model_meta") and callable(module.model_meta):
                module.model_meta()
            else:
                raise Exception('function "model_meta" does not exist')
            tf_saver = tf.train.Saver()
        else:
            _LOGGER.error("Neither .meta nor model.py exist, can't import meta")
            raise Exception("Neither .meta nor model.py exist, can't import meta")

        tf_saver.restore(save_path=model_path, sess=session)

        _LOGGER.info('After load checkpoint')
        return self._update_tensors(session)

    def _load_model_from_script(self, script):
        pass

    def _update_tensors(self, session):
        graph = session.graph
        input_objs = [_Input(graph.get_tensor_by_name(name), data_format) for name, data_format in
                      zip(self.input_names, self.input_formats)]
        output_objs = [_Output(graph.get_tensor_by_name(name)) for name in self.output_names]

        _LOGGER.info('_update_tensors: inputs: %s, outputs: %s', input_objs, output_objs)
        for i in input_objs:
            _LOGGER.info('dump input: %s', i)
        for output in output_objs:
            _LOGGER.info('dump output: %s', output)

        return input_objs, output_objs


class FrozenGraphLoader(CheckpointLoader):
    """
    Load frozen graph, .pb file
    """

    def __init__(self, input_names, output_names, input_formats):
        _LOGGER.info('Construct FrozenGraphLoader, inputs: %s, outputs: %s, input formats: %s', input_names,
                     output_names, input_formats)
        super(FrozenGraphLoader, self).__init__(input_names, output_names, input_formats)

    def _do_load(self, session, model_path, script_path):
        _LOGGER.info('Begin to load frozen graph, data format: %s, frozen graph path: %s', K.image_data_format(),
                     model_path)
        with tf.gfile.GFile(model_path, "rb") as graph_file:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(graph_file.read())
            tf.import_graph_def(graph_def, name="")
        _LOGGER.info('After load frozen graph')
        return self._update_tensors(session)


def _get_layer_information(model, layer_name):
    """
    Get the layer name, layer dtype and layer shape in the graph of ONNX model by the layer name
    :param model: the ONNX model of PyTorch
    :param layer_name: the layer name of ONNX model
    :return: the name, dtype, shape of layer
    """

    # type_table come from onnx-ml.proto: https://github.com/onnx/onnx/blob/master/onnx/onnx-ml.proto
    type_table = {0: 'UNDEFINED', 1: 'FLOAT', 2: 'INT', 3: 'STRING', 4: 'TENSOR', 5: 'GRAPH', 6: 'FLOATS',
                  7: 'INT', 8: 'STRINGS', 9: 'TENSORS', 10: 'GRAPHS'}

    layer_information = list(model.graph.value_info)
    layer_information.append(model.graph.input[0])
    layer_information.append(model.graph.output[0])
    layer_information_table = {item.name: item for item in layer_information}

    if layer_name in layer_information_table:
        item = layer_information_table[layer_name]
        elem_type = item.type.tensor_type.elem_type
        layer_dim = item.type.tensor_type.shape.dim
        layer_dtype = type_table[elem_type]
        layer_shape = [i.dim_value for i in layer_dim]
        return item.name, layer_shape[1:], layer_dtype, elem_type
    else:
        raise Exception('The layer name: {} is not in the model'.format(layer_name))


class _InputOnnx:
    """
    ONNX input description, for export config file
    """

    def __init__(self, model, input_name, input_format):
        self.name, self.shape, self.dtype, self.onnx_datatype_enum = _get_layer_information(model, input_name)
        self.data_format = input_format

    def __str__(self):
        return '(Input name: %s, shape: %s, dtype: %s, data_format: %s, onnx_datatype_enum: %s)' % \
               (self.name, self.shape, self.dtype, self.data_format, self.onnx_datatype_enum)


class _OutputOnnx:
    """
    ONNX output description, for export config file
    """

    def __init__(self, model, input_name):
        self.name, self.shape, self.dtype, self.onnx_datatype_enum = _get_layer_information(model, input_name)

    def __str__(self):
        return '(Output name: %s, shape: %s, dtype: %s, onnx_datatype_enum: %s)' % \
               (self.name, self.shape, self.dtype, self.onnx_datatype_enum)


class OnnxLoader:
    """
    Load PyTorch model, .onnx file
    """

    def __init__(self, input_names, output_names, input_formats):
        _LOGGER.info('Construct OnnxLoader, inputs: %s, outputs: %s, input formats: %s', input_names, output_names,
                     input_formats)
        self.input_names = input_names
        self.output_names = output_names
        self.input_formats = input_formats

    def load_model(self, onnx_path, script_path, _):
        """
        Load ONNX model
        :param onnx_path: model path, *.onnx
        :param script_path: model.py
        :param _: do nothing， keep the interface uniform
        :return: None, inputs and outputs
        """

        _LOGGER.info('Begin to load ONNX, data format: %s, ONNX path: %s, script path: %s', K.image_data_format(),
                     onnx_path, script_path)
        model = onnx.load(onnx_path)
        model = onnx.utils.polish_model(model)
        _LOGGER.info('After load ONNX, the nodes of model: %s', model.graph.node)
        inputs, outputs = self._update_tensors(model)
        return None, inputs, outputs

    def _update_tensors(self, model):
        """
        ONNX description, for export config file
        """

        inputs = [_InputOnnx(model, input_name, input_format) for input_name, input_format in
                  zip(self.input_names, self.input_formats)]
        outputs = [_OutputOnnx(model, output_name) for output_name in self.output_names]

        _LOGGER.info('_update_tensors: inputs: %s, outputs: %s', inputs, outputs)
        for i in inputs:
            _LOGGER.info('dump input: %s', i)
        for output in outputs:
            _LOGGER.info('dump output: %s', output)
        return inputs, outputs
