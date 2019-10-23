# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Model compiler base class
"""
from abc import abstractmethod
import os

import tensorflow as tf

from .compressor import compress_dir
from .model_info import ModelInfo
from .model_loader import CheckpointLoader, FrozenGraphLoader, KerasModelLoader, OnnxLoader
from .model_proto_exporter import ModelProtoExporter
from ..log_util import get_logger
from ..message import fail, success

_LOGGER = get_logger(__name__)


class BaseCompiler:
    """
    Compiler base class
    """
    _COMMON_PARAMS = [
        "h5_path",
        "checkpoint_path",
        "frozen_graph_path",
        "onnx_path",
        "script_path",
        "model_name",
        "input_layer_names",
        "output_layer_names",
        "input_names",
        "input_formats",
        "output_names",
        "export_path",
        "max_batch_size"
    ]
    _COMMON_REQUIRED = [
        "model_name",
        "export_path",
        "max_batch_size"
    ]

    def __init__(self, config):
        for item in self._COMMON_PARAMS:
            if config.get_attribute(item) is None and item in self._COMMON_REQUIRED:
                _LOGGER.error('Require "%s" but not found', item)
                raise Exception('Require "%s" but not found' % item)
            self.__setattr__(item, config.get_attribute(item))

        self.source_type, self.model_path = self._get_source_model()
        self.model_info = ModelInfo(self.model_name, self.max_batch_size, self.source_type)
        self.target_dir = self._make_target_dir()
        self.version = self._get_model_version(self.target_dir)
        self.version_dir = self._make_version_dir()
        self.inputs = []
        self.outputs = []
        self.custom_object = None
        _LOGGER.info('Output dir is: %s, version: %s', self.target_dir, self.version)

    def _get_source_model(self):
        if self.h5_path is not None and os.path.exists(self.h5_path):
            source_type = 'H5'
            model_path = self.h5_path
        elif self.frozen_graph_path is not None and os.path.exists(self.frozen_graph_path):
            source_type = 'Frozen Graph'
            model_path = self.frozen_graph_path
        elif self.onnx_path is not None and os.path.exists(self.onnx_path):
            source_type = 'ONNX'
            model_path = self.onnx_path
        elif self.checkpoint_path:
            source_type = 'Checkpoint'
            model_path = self.checkpoint_path
        else:
            raise ImportError('Can not get the input model')
        return source_type, model_path

    def compile(self):
        """
        Compile model
        :return: Return compile result
        """
        try:
            self._do_compile()
            self._export_config()
            zip_path = self._compress()
            return success(zip_path)
        except Exception as error:  # pylint:disable=broad-except
            _LOGGER.error('Compile model failure, error: %s', error)
            _LOGGER.exception(error)
            return fail(str(error))

    def _export_config(self):
        """
        Export config.pbtxt, which use to do inference in servinglite
        :return:
        """
        exporter = ModelProtoExporter(self.target_dir, self.model_info, self.get_platform)
        exporter.export()

    def _compress(self):
        """
        Compress model to .zip
        :return:
        """
        # self.target_dir -> model_name.zip
        zip_file_path = os.path.join(self.export_path, self.model_name + '.zip')
        return compress_dir(self.target_dir, zip_file_path)

    def _do_compile(self):

        if self.source_type == 'ONNX':
            loader = OnnxLoader(self.input_names, self.output_names, self.input_formats)
        elif self.source_type == 'H5':
            loader = KerasModelLoader(self.input_layer_names, self.output_layer_names)
        elif self.source_type == 'Frozen Graph':
            loader = FrozenGraphLoader(self.input_names, self.output_names, self.input_formats)
        else:
            loader = CheckpointLoader(self.input_names, self.output_names, self.input_formats)
        custom_object, inputs, outputs = loader.load_model(self.model_path, self.script_path, self._after_load_model)
        self.model_info.update(custom_object, inputs, outputs)
        self._after_end_session(self.model_info)

    @abstractmethod
    def _after_load_model(self, session, inputs, outputs):
        pass

    @abstractmethod
    def _after_end_session(self, model_info):
        pass

    @abstractmethod
    def get_platform(self):
        """
        Get platform
        :return: platform and version
        """
        pass

    @staticmethod
    def _to_frozen_graph(session, frozen_pb_path, outputs):
        _LOGGER.info('Begin to convert to frozen graph.')
        graph_def = tf.compat.v1.graph_util.convert_variables_to_constants(session, session.graph_def,
                                                                           [o.name for o in outputs])
        graph_def = tf.compat.v1.graph_util.remove_training_nodes(graph_def)
        with open(frozen_pb_path, 'wb') as frozen_pb_file:
            frozen_pb_file.write(graph_def.SerializeToString())
        _LOGGER.info('_to_frozen_graph:: convert to frozen graph success, output file: %s', frozen_pb_path)
        return frozen_pb_path

    def _make_target_dir(self):
        """
        Make target dir, the structure of export dir is:
        export_dir
        └── model_name(target_dir)
            ├── config.pbtxt
            ├── version_1(version_dir)
            │   └── serving model, TensorRT model or others
            └── version_2
                └── serving model, TensorRT model or others
        :return:
        """
        _LOGGER.info('make_target_dir: export base path: %s', self.export_path)
        if not os.path.exists(self.export_path):
            os.makedirs(self.export_path, exist_ok=True)
        target_dir = os.path.join(self.export_path, self.model_name)
        os.makedirs(target_dir, exist_ok=True)
        return target_dir

    def _make_version_dir(self):
        version_dir = os.path.join(self.target_dir, self.version)
        if os.path.exists(version_dir):
            raise Exception('Output version is already exist: {}'.format(version_dir))
        return version_dir

    def _get_model_version(self, model_dir):
        try:
            version = getattr(self, 'version')
            if isinstance(version, int):
                version = str(version)
            return version
        except AttributeError:
            sub_dirs = [fold for fold in os.listdir(model_dir)
                        if os.path.isdir(os.path.join(model_dir, fold)) and fold.isdigit()]
            sub_dirs.sort()
            version = str(int(sub_dirs[-1]) + 1) if sub_dirs else "1"
            return version
