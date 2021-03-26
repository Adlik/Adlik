# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
from adlik_serving import PredictContext
import numpy as np

YOLOV3_HW = (320, 320)


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    image_data = np.load('./ppyolo_inputs/input_0.npy')
    input_size = np.load('./ppyolo_inputs/input_1.npy')

    input_name1 = context.model_config.input[0].name
    input_name2 = context.model_config.input[1].name
    output_name1 = context.model_config.output[0].name
    output_name2 = context.model_config.output[1].name

    cur_idx = 0
    num_of_images = len(image_data)

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs_1 = image_data[cur_idx:cur_idx + batch_size]
            inputs_2 = input_size[cur_idx:cur_idx + batch_size]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs_1, next_inputs_2 = _next_batch(batch_size - (num_of_images - image_idx))
            inputs_1 = image_data[image_idx:] + next_inputs_1
            inputs_2 = input_size[image_idx:] + next_inputs_2

        return inputs_1, inputs_2

    num_of_batches = 1
    if num_of_images % FLAGS.batch_size != 0:
        num_of_batches += 1

    for _ in range(num_of_batches):
        i_inputs_1, i_inputs_2 = _next_batch(FLAGS.batch_size)
        context.run(inputs={input_name1: i_inputs_1, input_name2: i_inputs_2},
                    outputs=[output_name1, output_name2],
                    batch_size=FLAGS.batch_size)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='yolo',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('image_filename', type=str, nargs='?', help='Input image.')
    FLAGS = parser.parse_args()
    _main()
