# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
import os
import time
import logging

from PIL import Image
from adlik_serving import PredictContext, model_config_pb2, tensor_dtype_to_np_dtype
import numpy as np

FLAGS = None

ISOTIMEFORMAT = '%Y-%m-%d %H:%M:%S,%f'


def _parse_model(config, model_name, batch_size):
    if config.max_batch_size == 0:
        if batch_size != 1:
            raise Exception("batching not supported for model '" + model_name + "'")
    else:  # max_batch_size > 0
        if batch_size > config.max_batch_size:
            raise Exception(
                "expecting batch size <= {} for model '{}'".format(config.max_batch_size, model_name))

    input_ = config.input[0]
    output = config.output[0]
    if input_.format == model_config_pb2.ModelInput.FORMAT_NHWC:
        h = input_.dims[0]
        w = input_.dims[1]
        c = input_.dims[2]
    else:
        c = input_.dims[0]
        h = input_.dims[1]
        w = input_.dims[2]

    return input_.name, output.name, c, h, w, input_.format, tensor_dtype_to_np_dtype(input_.data_type)


def _gen_input_data(data_format, dtype, c, h, w):
    if os.path.isdir(FLAGS.image_filename):
        file_names = [os.path.join(FLAGS.image_filename, f)
                      for f in os.listdir(FLAGS.image_filename)
                      if os.path.isfile(os.path.join(FLAGS.image_filename, f))]
    else:
        file_names = [FLAGS.image_filename]

    file_names.sort()

    image_data = []
    for filename in file_names:
        img = Image.open(filename)
        array = _preprocess(img, data_format, dtype, c, h, w)
        image_data.append(array)
    return file_names, image_data


def _preprocess(img, data_format, dtype, c, h, w):
    if c == 3:
        half_the_width = img.size[0] / 2
        half_the_height = img.size[1] / 2
        img4 = img.crop(
            (
                half_the_width - 112,
                half_the_height - 112,
                half_the_width + 112,
                half_the_height + 112
            )
        )
        sample_img = img4.convert('RGB')
    else:
        raise Exception('Imagenet image channel must be 3, bug not {}'.format(c))

    resized_img = sample_img.resize((h, w), Image.BILINEAR)
    resized = np.array(resized_img).reshape((h, w, 3))

    scaled = resized.astype(dtype) / 255.0

    # Swap to CHW if necessary
    if data_format == model_config_pb2.ModelInput.FORMAT_NCHW:
        ordered = np.transpose(scaled, (2, 0, 1))
    else:
        ordered = scaled
    return ordered


def _postprocess(results, file_names, batch_size):
    if len(results.tensor) != len(file_names):
        raise Exception("expected {} results, got {}".format(batch_size, len(results)))
    if len(file_names) != batch_size:
        raise Exception("expected {} file names, got {}".format(batch_size, len(file_names)))

    if results.batch_classes:
        for i in range(batch_size):
            print("Image: '{}', result: {}".format(file_names[i],
                                                   results.batch_classes[i]))
    else:
        print("response doesn't contain 'batch classes' field, get class information from 'tensor' field!")
        for i in range(batch_size):
            print("Image: '{}', result: {}".format(file_names[i], np.argmax(results.tensor[i])))


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    model_config = context.model_config

    input_name, output_name, c, h, w, data_format, dtype = _parse_model(
        model_config, FLAGS.model_name, FLAGS.batch_size)

    file_names, image_data = _gen_input_data(data_format, dtype, c, h, w)

    cur_idx = 0
    num_of_images = len(image_data)

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs = image_data[cur_idx:cur_idx + batch_size]
            outputs = file_names[cur_idx:cur_idx + batch_size]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs, next_outputs = _next_batch(batch_size - (num_of_images - image_idx))
            inputs = image_data[image_idx:] + next_inputs
            outputs = file_names[image_idx:] + next_outputs

        return inputs, outputs

    num_of_batches = 99
    if num_of_images % FLAGS.batch_size != 0:
        num_of_batches += 1
    logging.basicConfig(level=logging.DEBUG,
                        filename=os.path.join(FLAGS.log_path, 'client_time.log'),
                        filemode='a',
                        format='%(asctime)s - %(pathname)s[line:%(lineno)d] - %(levelname)s: %(message)s')
    for _ in range(num_of_batches):
        i_inputs, i_outputs = _next_batch(FLAGS.batch_size)
        time1 = time.time()
        context.run(inputs={input_name: i_inputs},
                    outputs={output_name: FLAGS.classes},
                    batch_size=FLAGS.batch_size)
        logging.info(f'The time of predict: {time.time() - time1}')
        print(f'{_} / {num_of_batches}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='resnet50',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-c', '--classes', type=int, required=False, default=1,
                        help='Number of class results to report. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('image_filename', type=str, nargs='?',
                        help='Input image.')
    parser.add_argument('-l', '--log-path', default='/home/john/Adlik', type=str, help='Log path')
    FLAGS = parser.parse_args()
    _main()
