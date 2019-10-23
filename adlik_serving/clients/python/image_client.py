# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik serving prediction,
use model resnet50 which uses tensorRT platform
"""

import argparse
import os

from PIL import Image
import numpy as np
from adlik_serving import PredictContext, model_config_pb2 as model_config, tensor_dtype_to_np_dtype
from tensorflow.core.framework import types_pb2

FLAGS = None


def parse_model(config, model_name, batch_size):
    """
    Check the configuration of a model to make sure it meets the
    requirements for an image classification network (as expected by
    this client)
    """
    if config.name != model_name:
        raise Exception("Get model config exception, config name: {}".format(config.name))

    if len(config.input) != 1:
        raise Exception("expecting 1 input, got {}".format(len(config.input)))
    if len(config.output) != 1:
        raise Exception("expecting 1 output, got {}".format(len(config.output)))

    input_ = config.input[0]
    output = config.output[0]

    if output.data_type != types_pb2.DT_FLOAT:
        raise Exception("expecting output data type to be DT_FLOAT, model '" +
                        model_name + "' output type is " +
                        model_config.DataType.Name(output.data_type))

    # Output is expected to be a vector. But allow any number of
    # dimensions as long as all but 1 is size 1 (e.g. { 10 }, { 1, 10
    # }, { 10, 1, 1 } are all ok).
    non_one_cnt = 0
    for dim in output.dims:
        if dim > 1:
            non_one_cnt += 1
            if non_one_cnt > 1:
                raise Exception("expecting model output to be a vector")

    # Model specifying maximum batch size of 0 indicates that batching
    # is not supported and so the input tensors do not expect an "N"
    # dimension (and 'batch_size' should be 1 so that only a single
    # image instance is inferred at a time).
    max_batch_size = config.max_batch_size
    if max_batch_size == 0:
        if batch_size != 1:
            raise Exception("batching not supported for model '" + model_name + "'")
    else:  # max_batch_size > 0
        if batch_size > max_batch_size:
            raise Exception(
                "expecting batch size <= {} for model '{}'".format(max_batch_size, model_name))

    # Model input must have 3 dims, either CHW or HWC
    if len(input_.dims) != 3:
        raise Exception(
            "expecting input to have 3 dimensions, model '{}' input has {}".format(
                model_name, len(input_.dims)))

    if ((input_.format != model_config.ModelInput.FORMAT_NCHW) and
            (input_.format != model_config.ModelInput.FORMAT_NHWC)):
        raise Exception("unexpected input format " + model_config.ModelInput.Format.Name(input_.format) +
                        ", expecting " +
                        model_config.ModelInput.Format.Name(model_config.ModelInput.FORMAT_NCHW) +
                        " or " +
                        model_config.ModelInput.Format.Name(model_config.ModelInput.FORMAT_NHWC))

    if input_.format == model_config.ModelInput.FORMAT_NHWC:
        h = input_.dims[0]
        w = input_.dims[1]
        c = input_.dims[2]
    else:
        c = input_.dims[0]
        h = input_.dims[1]
        w = input_.dims[2]

    output_size = 1
    for dim in output.dims:
        output_size = output_size * dim
    output_size = output_size * np.dtype(tensor_dtype_to_np_dtype(output.data_type)).itemsize

    return input_.name, output.name, c, h, w, input_.format, tensor_dtype_to_np_dtype(input_.data_type), output_size


def preprocess(img, data_format, dtype, c, h, w, scaling):
    """
    Pre-process an image to meet the size, type and format
    requirements specified by the parameters.
    """
    # np.set_printoptions(threshold='nan')

    if c == 1:
        sample_img = img.convert('L')
    else:
        sample_img = img.convert('RGB')

    resized_img = sample_img.resize((h, w), Image.BILINEAR)
    resized = np.array(resized_img)
    if resized.ndim == 2:
        resized = resized[:, :, np.newaxis]

    typed = resized.astype(dtype)

    if scaling == 'INCEPTION':
        scaled = (typed / 128) - 1
    elif scaling == 'VGG':
        if c == 1:
            scaled = typed - np.asarray((128,), dtype=dtype)
        else:
            scaled = typed - np.asarray((123, 117, 104), dtype=dtype)
    elif scaling == 'MNIST':
        scaled = typed / 255.0
    else:
        scaled = typed

    # Swap to CHW if necessary
    if data_format == model_config.ModelInput.FORMAT_NCHW:
        ordered = np.transpose(scaled, (2, 0, 1))
    else:
        ordered = scaled

    # Channels are in RGB order. Currently model configuration data
    # doesn't provide any information as to other channel orderings
    # (like BGR) so we just assume RGB.
    return ordered


def postprocess(results, filenames, batch_size):
    """
    Post-process results to show classifications.
    """
    if len(results.tensor) != len(filenames):
        raise Exception("expected {} results, got {}".format(batch_size, len(results)))
    if len(filenames) != batch_size:
        raise Exception("expected {} filenames, got {}".format(batch_size, len(filenames)))

    print("Predict results are: ")
    if results.batch_classes:
        for i in range(batch_size):
            print("Image: '{}', result: {}".format(filenames[i],
                                                   results.batch_classes[i]))
    else:
        print("response doesn't contain 'batch classes' field, get class information from 'tensor' field!")
        for i in range(batch_size):
            print("Image: '{}', result: {}".format(filenames[i], np.argmax(results.tensor[i])))


def _main():
    credentials = None
    if FLAGS.protocol == 'grpc' and FLAGS.credentials is not None:
        with open(FLAGS.credentials, 'rb') as f:
            credentials = f.read()

    context = PredictContext(FLAGS.model_name, model_version=FLAGS.model_version, signature=FLAGS.signature,
                             url=FLAGS.url, protocol=FLAGS.protocol, grpc_domain=FLAGS.grpc_domain,
                             credentials=credentials, verbose=FLAGS.verbose)

    input_name, output_name, c, h, w, data_format, dtype, output_size = parse_model(
        context.model_config, FLAGS.model_name, FLAGS.batch_size)
    print('Input name: {}, output name: {}, (c, h, w)=({}, {}, {}), data_format: {}, dtype: {}, output size: {}'.format(
        input_name, output_name, c, h, w, data_format, dtype, output_size))

    if os.path.isdir(FLAGS.image_filename):
        filenames = [os.path.join(FLAGS.image_filename, f)
                     for f in os.listdir(FLAGS.image_filename)
                     if os.path.isfile(os.path.join(FLAGS.image_filename, f))]
    else:
        filenames = [FLAGS.image_filename, ]

    filenames.sort()

    image_data = []
    for filename in filenames:
        img = Image.open(filename)
        array = preprocess(img, data_format, dtype, c, h, w, FLAGS.scaling)
        image_data.append(array)

    cur_idx = 0
    num_of_images = len(image_data)

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs = image_data[cur_idx:cur_idx + batch_size]
            outputs = filenames[cur_idx:cur_idx + batch_size]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs, next_outputs = _next_batch(batch_size - (num_of_images - image_idx))
            inputs = image_data[image_idx:] + next_inputs
            outputs = filenames[image_idx:] + next_outputs

        return inputs, outputs

    num_of_batches = num_of_images // FLAGS.batch_size
    if num_of_images % FLAGS.batch_size != 0:
        num_of_batches += 1

    if FLAGS.protocol == 'grpc' and FLAGS.is_async:
        print("async inference ")
        requests = []
        batchOutputs = []
        # async submit input data use the grpc inferface
        for _ in range(num_of_batches):
            i_inputs, i_outputs = _next_batch(FLAGS.batch_size)
            batchOutputs.append(i_outputs)
            requests.append(context.async_run(inputs={input_name: i_inputs},
                                              outputs={output_name: FLAGS.classes},
                                              batch_size=FLAGS.batch_size))
        for i in range(len(requests)):
            response = requests[i].result()
            result = context.process_response({output_name: FLAGS.classes}, response)
            postprocess(result[output_name], batchOutputs[i], FLAGS.batch_size)

    else:
        for _ in range(num_of_batches):
            i_inputs, i_outputs = _next_batch(FLAGS.batch_size)
            result = context.run(inputs={input_name: i_inputs},
                                 outputs={output_name: FLAGS.classes},
                                 batch_size=FLAGS.batch_size)
            postprocess(result[output_name], i_outputs, FLAGS.batch_size)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=True,
                        help='Enable verbose output')
    parser.add_argument('-m', '--model-name', type=str, required=True,
                        help='Name of model')
    parser.add_argument('-x', '--model-version', type=int, required=False, default=None,
                        help='Version of model. Default is to use latest version.')
    parser.add_argument('-g', '--signature', type=str, required=False, default='predict',
                        help='Signature of model. Default is to use "serving_default".')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-c', '--classes', type=int, required=False, default=1,
                        help='Number of class results to report. Default is 1.')
    parser.add_argument('-s', '--scaling', type=str, choices=['NONE', 'INCEPTION', 'VGG'],
                        required=False, default='NONE',
                        help='Type of scaling to apply to image pixels. Default is NONE.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Adlik serving server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('-d', '--grpc-domain', type=str, required=False, default=None,
                        help='Grpc domain when use grpc protocol. Default is None.')
    parser.add_argument('-n', '--async', type=bool, required=False, default=False,
                        help='Async to run the request when use grpc protocol. Default is No.', dest='is_async')
    parser.add_argument('-a', '--credentials', type=str, required=False,
                        default=None,
                        help='Path of credentials, required when grpc SSL-enabled. Default is None.')
    parser.add_argument('image_filename', type=str, nargs='?', default=None,
                        help='Input image.')
    FLAGS = parser.parse_args()
    _main()
