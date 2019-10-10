# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik serving prediction,
use model plan_float16_float32_float32 which runs on tensorRT platform
"""

import argparse
import sys

import numpy as np
from adlik_serving import PredictContext

FLAGS = None

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=False,
                        help='Enable verbose output')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Adlik serving server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')

    FLAGS = parser.parse_args()
    protocol = FLAGS.protocol

    # We use a simple model that takes 2 input tensors of 16 integers
    # each and returns 2 output tensors of 16 integers each. One
    # output tensor is the element-wise sum of the inputs and one
    # output is the element-wise difference.
    model_name = "plan_float16_float32_float32"
    model_version = 1
    batch_size = 1

    input0_data = np.arange(start=0, stop=16, dtype=np.float16)
    input1_data = np.ones(shape=16, dtype=np.float16)

    context = PredictContext(model_name, model_version=model_version, url=FLAGS.url, protocol=FLAGS.protocol)

    result = context.run(inputs={'INPUT0': (input0_data,),
                                 'INPUT1': (input1_data,)},
                         outputs=['OUTPUT0', 'OUTPUT1'],
                         batch_size=batch_size)
    output0_data = result['OUTPUT0'].tensor[0]
    output1_data = result['OUTPUT1'].tensor[0]

    for i in range(16):
        print(str(input0_data[i]) + " + " + str(input1_data[i]) + " = " + str(output0_data[i]))
        print(str(input0_data[i]) + " - " + str(input1_data[i]) + " = " + str(output1_data[i]))
        if (input0_data[i] + input1_data[i]) != output0_data[i]:
            print("error: incorrect sum")
            sys.exit(1)
        if (input0_data[i] - input1_data[i]) != output1_data[i]:
            print("error: incorrect difference")
            sys.exit(1)
