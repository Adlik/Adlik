# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
import os

from PIL import Image
from adlik_serving import PredictContext, model_config_pb2, tensor_dtype_to_np_dtype
import numpy as np

FLAGS = None
YOLOV3_HW = (320, 320)
LABEL_FILE_PATH = os.path.join(os.path.dirname(__file__), 'coco_labels.txt')


def load_label_categories(label_file_path):
    categories = [line.rstrip('\n') for line in open(label_file_path)]
    return categories


ALL_CATEGORIES = load_label_categories(LABEL_FILE_PATH)

# Let's make sure that there are 80 classes, as expected for the COCO data set:
CATEGORY_NUM = len(ALL_CATEGORIES)
assert CATEGORY_NUM == 80


def _parse_model(config, model_name, batch_size):
    if config.max_batch_size == 0:
        if batch_size != 1:
            raise Exception("batching not supported for model '" + model_name + "'")
    else:  # max_batch_size > 0
        if batch_size > config.max_batch_size:
            raise Exception(
                "expecting batch size <= {} for model '{}'".format(config.max_batch_size, model_name))

    input_ = config.input[0]

    output_name = [output.name for output in config.output]
    if input_.format == model_config_pb2.ModelInput.FORMAT_NHWC:
        h = input_.dims[0]
        w = input_.dims[1]
        c = input_.dims[2]
    else:
        c = input_.dims[0]
        h = input_.dims[1]
        w = input_.dims[2]
    return input_.name, output_name, c, h, w, input_.format, tensor_dtype_to_np_dtype(input_.data_type)


def _gen_input_data():
    if os.path.isdir(FLAGS.image_filename):
        file_names = [os.path.join(FLAGS.image_filename, f)
                      for f in os.listdir(FLAGS.image_filename)
                      if os.path.isfile(os.path.join(FLAGS.image_filename, f))]
    else:
        file_names = [FLAGS.image_filename]
    file_names.sort()

    image_data = []
    for filename in file_names:
        image = _process(filename)
        image_data.append(image)
    return file_names, image_data


def _process(file_name):
    def _load_and_resize(input_image_path):
        image = Image.open(input_image_path)
        img_resized = image.resize(YOLOV3_HW, resample=Image.BICUBIC)

        img_resized = np.array(img_resized, dtype=np.float32, order='C')
        return img_resized

    def _shuffle_and_normalize(image):
        image /= 255.0
        # HWC to CHW format:
        image = np.transpose(image, [2, 0, 1])
        # Convert the image to row-major order, also known as "C order":
        image = np.array(image, dtype=np.float32, order='C')
        return image

    image_resized = _load_and_resize(file_name)
    image_preprocessed = _shuffle_and_normalize(image_resized)
    return image_preprocessed


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    model_config = context.model_config

    input_names, output_names, c, h, w, data_format, dtype = _parse_model(
        model_config, FLAGS.model_name, FLAGS.batch_size)

    file_names, image_data = _gen_input_data()

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

    for _ in range(num_of_batches):
        i_inputs, i_outputs = _next_batch(FLAGS.batch_size)
        context.run(inputs={input_names: i_inputs},
                    outputs=output_names,
                    batch_size=FLAGS.batch_size)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='yolov3',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-c', '--classes', type=int, required=False, default=80,
                        help='Number of class results to report. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('image_filename', type=str, nargs='?', help='Input image.')
    FLAGS = parser.parse_args()
    _main()
