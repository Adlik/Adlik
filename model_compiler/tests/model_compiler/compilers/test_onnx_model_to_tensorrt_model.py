# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from unittest import TestCase
from tempfile import TemporaryDirectory, NamedTemporaryFile

from torch.autograd import Variable
import torchvision
import torch.onnx
import torch
import os

import pytest
import tensorflow as tf
import numpy as np
from PIL import Image
from tensorflow.core.framework.types_pb2 import DataType as TfDataType  # pylint: disable=no-name-in-module
from tensorrt import ICudaEngine

import model_compiler.compilers.onnx_model_to_tensorrt_model as compiler
import model_compiler.compilers.onnx_model_file_to_onnx_model as onnx_compiler
import model_compiler.compilers.tf_frozen_graph_model_to_onnx_model as tf_onnx_compiler
import model_compiler.compilers.tf_model_to_tf_frozen_graph_model as frozen_graph_compiler
from model_compiler.compilers.onnx_model_to_tensorrt_model import Config as TensorrtConfig
from model_compiler.compilers.onnx_model_file_to_onnx_model import Config as OnnxConfig
from model_compiler.models.irs.tf_model import Input as TfInput, TensorFlowModel
from model_compiler.protos.generated.model_config_pb2 import ModelInput, ModelOutput
from model_compiler.compilers.onnx_model_file_to_onnx_model import DataFormat
from model_compiler.models.sources.onnx_model_file import ONNXModelFile


class ConfigTestCase(TestCase):
    def test_from_json(self):
        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=False),
                         TensorrtConfig.from_json({'max_batch_size': 7}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=False),
                         TensorrtConfig.from_json({'max_batch_size': 7, 'data_type': None}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type='FP16', enable_strict_types=False),
                         TensorrtConfig.from_json({'max_batch_size': 7, 'data_type': 'FP16'}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=True),
                         TensorrtConfig.from_json({'max_batch_size': 7, 'enable_strict_types': True}))

    def test_from_env(self):
        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=False),
                         TensorrtConfig.from_env({'MAX_BATCH_SIZE': '7'}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=False),
                         TensorrtConfig.from_env({'MAX_BATCH_SIZE': '7', 'DATA_TYPE': ''}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type='FP16', enable_strict_types=False),
                         TensorrtConfig.from_env({'MAX_BATCH_SIZE': '7', 'DATA_TYPE': 'FP16'}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=False),
                         TensorrtConfig.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_STRICT_TYPES': '0'}))

        self.assertEqual(TensorrtConfig(max_batch_size=7, data_type=None, enable_strict_types=True),
                         TensorrtConfig.from_env({'MAX_BATCH_SIZE': '7', 'ENABLE_STRICT_TYPES': '1'}))


def _make_onnx_model(func, batch_size_1, batch_size_2):
    with tf.Graph().as_default(), tf.compat.v1.Session().as_default() as session:
        input_x = tf.compat.v1.placeholder(dtype=tf.float32, shape=[batch_size_1, 4], name='x')
        input_y = tf.compat.v1.placeholder(dtype=tf.float32, shape=[batch_size_2, 4], name='y')
        output_z = func(input_x, input_y, session)

    frozen_graph_model = frozen_graph_compiler.compile_source(
        source=TensorFlowModel(inputs=[TfInput(tensor=input_x), TfInput(tensor=input_y)],
                               outputs=[output_z],
                               session=session)
    )

    return tf_onnx_compiler.compile_source(frozen_graph_model)


def _make_resnet50_onnx_model_file(file_path):
    device = torch.device('cpu')
    dummy_input = Variable(torch.randn(1, 3, 224, 224))
    dummy_input = dummy_input.to(device)
    model = torchvision.models.resnet50(pretrained=True)
    resnet50_onnx_model_path = os.path.join(file_path, 'model', 'resnet50.onnx')
    dir_name = os.path.dirname(resnet50_onnx_model_path)
    os.makedirs(dir_name, exist_ok=True)
    torch.onnx.export(model, dummy_input, resnet50_onnx_model_path, verbose=True, keep_initializers_as_inputs=True)
    return resnet50_onnx_model_path


def _make_calibration_dataset_simple(data_path):
    img_np = np.random.rand(500, 398, 3) * 255.0
    img = Image.fromarray(img_np.astype(np.uint8))
    calibration_data_path = os.path.join(data_path, 'calibration_dataset_sample', 'calibration_data.JPEG')
    dir_name = os.path.dirname(calibration_data_path)
    os.makedirs(dir_name, exist_ok=True)
    img.save(calibration_data_path)
    calibration_data_txt = os.path.join(data_path, 'calibration_data.txt')
    with open(calibration_data_txt, 'w') as txt_file:
        txt_file.write('calibration_data.JPEG\n')
    return calibration_data_path


@pytest.mark.gpu_test
class CompileSourceTestCase(TestCase):
    def test_compile_simple(self):
        for batch_size in [3, None]:
            onnx_model = _make_onnx_model(func=lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'),
                                          batch_size_1=batch_size,
                                          batch_size_2=batch_size)

            compiled = compiler.compile_source(source=onnx_model, config=TensorrtConfig(max_batch_size=4))

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4]),
                              ModelInput(name='y:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4])])

            self.assertEqual(compiled.input_data_formats, [None, None])
            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z:0', data_type=TfDataType.DT_FLOAT, dims=[4])])
            self.assertIsInstance(compiled.cuda_engine, ICudaEngine)

    def test_compile_inconsistent_batch_size(self):
        onnx_model = _make_onnx_model(func=lambda input_x, input_y, _: tf.add(input_x, input_y, name='z'),
                                      batch_size_1=3,
                                      batch_size_2=None)

        with self.assertRaises(ValueError) as error:
            compiler.compile_source(source=onnx_model, config=TensorrtConfig(max_batch_size=4))

        self.assertEqual(error.exception.args, ('Inconsistent batch size specification.',))

    def test_compile_fp16(self):
        def _build_model(input_x, input_y, session):
            weight = tf.Variable(initial_value=0.0, dtype=tf.float32, name='w')

            session.run(weight.initializer)

            return tf.multiply(weight, input_x + input_y, name='z')

        for batch_size in [3, None]:
            onnx_model = _make_onnx_model(func=_build_model, batch_size_1=batch_size, batch_size_2=batch_size)

            compiled = compiler.compile_source(source=onnx_model,
                                               config=TensorrtConfig(max_batch_size=4,
                                                                     data_type='FP16',
                                                                     enable_strict_types=True))

            self.assertEqual(compiled.get_inputs(),
                             [ModelInput(name='x:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4]),
                              ModelInput(name='y:0', data_type=TfDataType.DT_FLOAT, format=None, dims=[4])])

            self.assertEqual(compiled.input_data_formats, [None, None])
            self.assertEqual(compiled.get_outputs(), [ModelOutput(name='z:0', data_type=TfDataType.DT_FLOAT, dims=[4])])
            self.assertIsInstance(compiled.cuda_engine, ICudaEngine)

    def test_compile_int8_default_calibrator(self):
        with TemporaryDirectory() as file_path:
            resnet50_onnx_model_file = _make_resnet50_onnx_model_file(file_path)
            resnet50_onnx_model = onnx_compiler.compile_source(source=ONNXModelFile(resnet50_onnx_model_file),
                                                               config=OnnxConfig(
                                                                      input_formats=[DataFormat.CHANNELS_FIRST]))
            calibration_dataset_sample = _make_calibration_dataset_simple(file_path)
            compiled = compiler.compile_source(source=resnet50_onnx_model,
                                               config=TensorrtConfig(max_batch_size=1, data_type='INT8',
                                                                     calibration_dataset=calibration_dataset_sample))
        self.assertEqual(compiled.get_inputs(), [ModelOutput(name='input.1', data_type=TfDataType.DT_FLOAT,
                                                             dims=[3, 224, 224])])
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_FIRST])
        self.assertEqual(compiled.get_outputs(), [ModelOutput(name='495', data_type=TfDataType.DT_FLOAT, dims=[4])])
        self.assertIsInstance(compiled.cuda_engine, ICudaEngine)

    def test_compile_int8_customize_calibrator(self):
        with TemporaryDirectory() as file_path, NamedTemporaryFile(mode='w+', suffix='.py') as calibrator_path:
            resnet50_onnx_model_file = _make_resnet50_onnx_model_file(file_path)
            resnet50_onnx_model = onnx_compiler.compile_source(source=ONNXModelFile(resnet50_onnx_model_file),
                                                               config=OnnxConfig(
                                                                   input_formats=[DataFormat.CHANNELS_FIRST]))
            calibration_dataset_sample = _make_calibration_dataset_simple(file_path)

            calibrator_path.write(
              "import tensorrt as trt\n"
              "import pycuda.driver as cuda\n"
              "import pycuda.autoinit\n"
              "import os\n"
              "import numpy as np\n"
              "from PIL import Image\n"
              "import torchvision.transforms as transforms\n"
              "\n\n"
              "class MyCalibrator(trt.IInt8EntropyCalibrator2):\n"
              "    def __init__(self, calibration_dataset, batch_size):\n"
              "        trt.IInt8EntropyCalibrator2.__init__(self)\n"
              "        self.cache_file = None\n"
              "        self.batch_size = batch_size\n"
              "        self.channel, self.width, self.height = 3, 224, 224\n"
              "        txt_file = open(os.path.join(calibration_dataset, '..', 'calibration_data.txt'), 'r')\n"
              "        image_lines = txt_file.readlines()\n"
              "        self.images = [os.path.join(calibration_dataset, image_line.split('\n')[0])"
              "                       for image_line in image_lines]\n"
              "        self.data_shape = (self.batch_size, self.channel, self.height, self.width)"
              "        self.data_size = trt.volume(list(self.data_shape)) * trt.float32.itemsize\n"
              "        self.current_index = 0"
              "        self.device_input = cuda.mem_alloc(self.data_size)\n"
              "\n"
              "    def get_batch_size(self):\n"
              "        return self.batch_size\n"
              "\n"
              "    def get_batch(self, names, p_str=None):\n"
              "        img_transform = transforms.Compose([transforms.Resize([self.height, self.width]),"
              "                                            transforms.ToTensor(), ])\n"
              "        batch_images = np.zeros(self.data_shape, dtype=np.float32)\n"
              "        img = Image.open(self.images[0])\n"
              "        batch_images[0] = img_transform(img).numpy()\n"
              "        if self.current_index + self.batch_size > len(batch_images):"
              "            return None"
              "        self.current_index += self.batch_size"
              "        cuda.memcpy_htod(self.device_input, batch_images.astype(np.float32))\n"
              "        return [int(self.device_input)]\n"
              "\n"
              "    def read_calibration_cache(self):\n"
              "        return self.cache_file\n"
              "\n"
              "    def write_calibration_cache(self, cache):\n"
              "        self.cache_file = cache\n")
            calibrator_path.seek(0)
            compiled = compiler.compile_source(source=resnet50_onnx_model,
                                               config=TensorrtConfig(max_batch_size=1, data_type='INT8',
                                                                     calibration_dataset=calibration_dataset_sample,
                                                                     calibrator=calibrator_path.name))
        self.assertEqual(compiled.get_inputs(), [ModelOutput(name='input.1', data_type=TfDataType.DT_FLOAT,
                                                             dims=[3, 224, 224])])
        self.assertEqual(compiled.input_data_formats, [DataFormat.CHANNELS_FIRST])
        self.assertEqual(compiled.get_outputs(), [ModelOutput(name='495', data_type=TfDataType.DT_FLOAT, dims=[4])])
        self.assertIsInstance(compiled.cuda_engine, ICudaEngine)
