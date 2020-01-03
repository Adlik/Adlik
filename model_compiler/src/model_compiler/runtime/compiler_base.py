# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Model compiler base class
"""
from abc import abstractmethod
import os
import shutil
import uuid

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
        "input_model",
        "script_path",
        "model_name",
        "input_layer_names",
        "output_layer_names",
        "input_names",
        "input_formats",
        "output_names",
        "export_path",
        "max_batch_size",
        "version"
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
        self.model_dir = self._make_model_dir()
        self.version, self.version_dir = self._get_version_dir()
        self.target_dir = self._make_target_dir()
        self.inputs = []
        self.outputs = []
        self.custom_object = None
        _LOGGER.info('Output dir is: %s, version: %s', self.model_dir, self.version)

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
        elif self.input_model:
            ex_name = os.path.splitext(self.input_model)[1].lower()
            types = {'.h5': 'H5', '.pb': 'Frozen Graph', '.pbtxt': 'Frozen Graph', '.onnx': 'ONNX',
                     '.ckpt': 'Checkpoint', '': 'Checkpoint'}
            source_type = types[ex_name]
            model_path = self.input_model
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
            config_path = self._export_config()
            os.rename(self.target_dir, self.version_dir)
            zip_path = self._compress([self.version_dir, config_path])
            return success(zip_path)
        except Exception as error:  # pylint:disable=broad-except
            _LOGGER.error('Compile model failure, error: %s', error)
            _LOGGER.exception(error)
            self._cleanup()
            return fail(str(error))

    def _export_config(self):
        """
        Export config.pbtxt, which use to do inference in servinglite
        :return:
        """
        exporter = ModelProtoExporter(self.model_dir, self.model_info, self.get_platform)
        return exporter.export()

    def _compress(self, source_list):
        """
        Compress model to .zip
        :return:
        """
        # self.target_dir -> modelName_version.zip
        zip_file_path = os.path.join(self.export_path, self.model_name + '_' + str(self.version) + '.zip')
        return compress_dir(source_list, zip_file_path)

    def _do_compile(self):
        if self.source_type == 'ONNX':
            loader = OnnxLoader(self.input_names, self.output_names, self.input_formats)
        elif self.source_type == 'H5':
            loader = KerasModelLoader(self)
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

    def _make_model_dir(self):
        """
        Make model dir, the structure of export dir is:
        export_dir
        └── model_name
            ├── config.pbtxt
            ├── version_1(version_dir)
            │   └── serving model, TensorRT model or others
            └── version_2
                └── serving model, TensorRT model or others
        :return:
        """
        _LOGGER.info('make_model_dir: export base path: %s', self.export_path)
        if not os.path.exists(self.export_path):
            os.makedirs(self.export_path, exist_ok=True)
        model_dir = os.path.join(self.export_path, self.model_name)
        os.makedirs(model_dir, exist_ok=True)
        return model_dir

    def _get_version_dir(self):
        version = getattr(self, "version", None)
        if version is None:
            version = self._get_model_default_version()
        version = str(version)
        version_dir = os.path.join(self.model_dir, version)
        _LOGGER.info("Export model version : %s, dir: %s", version, version_dir)
        if os.path.exists(version_dir):
            raise Exception('Output version is already exist: {}'.format(version_dir))
        return version, version_dir

    def _make_target_dir(self):
        temp_dir_name = str(uuid.uuid3(uuid.NAMESPACE_URL, '_'.join([self.model_name, self.version])))
        _LOGGER.info("temporary export dir: %s, %s", temp_dir_name, os.path.join(self.model_dir, temp_dir_name))
        return os.path.join(self.model_dir, temp_dir_name)

    def _get_model_default_version(self):
        sub_dirs = [int(child) for child in os.listdir(self.model_dir)
                    if os.path.isdir(os.path.join(self.model_dir, child)) and child.isdigit()]
        sub_dirs.sort()
        version = str(sub_dirs[-1] + 1) if sub_dirs else "1"
        return version

    def _cleanup(self):
        if os.path.exists(self.target_dir):
            shutil.rmtree(self.target_dir)
        if os.path.exists(self.version_dir):
            shutil.rmtree(self.version_dir)
