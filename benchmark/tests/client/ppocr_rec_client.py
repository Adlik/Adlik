# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
from adlik_serving import PredictContext
import numpy as np


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    image_data = []
    image = np.load('./input_0.npy')
    image_data.append(image)

    input_name = context.model_config.input[0].name
    output_name = 'save_infer_model/scale_0.tmp_1'

    cur_idx = 0
    num_of_images = len(image_data)

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs = image_data[cur_idx:cur_idx + batch_size]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs = _next_batch(batch_size - (num_of_images - image_idx))
            inputs = image_data[image_idx:] + next_inputs

        return inputs

    num_of_batches = 100

    for _ in range(num_of_batches):
        i_inputs = _next_batch(FLAGS.batch_size)
        context.run(inputs={input_name: i_inputs},
                    outputs=[output_name],
                    batch_size=FLAGS.batch_size)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='crnn',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:9500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('image_filename', type=str, nargs='?', help='Input image.')
    FLAGS = parser.parse_args()
    _main()
