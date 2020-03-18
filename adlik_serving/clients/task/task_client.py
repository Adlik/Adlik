# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample of create use grpc protocol, just for test grid and amc case
"""

import argparse
import random
import time

from adlik_serving.apis import task_pb2, task_service_pb2_grpc
import grpc

FLAGS = None


def _create_header():
    request = task_pb2.CreateTaskRequest()
    request.model_spec.name = FLAGS.model_name
    request.task_type = 1  # Training task
    request.is_sync = FLAGS.is_sync
    return request


def _create_grid_request():
    request = _create_header()
    if FLAGS.input and FLAGS.output:
        request.task.grid.input = FLAGS.input
        request.task.grid.output = FLAGS.output
        return request
    else:
        raise Exception("For grid case the following arguments are required: -i/--input and -o/--output")


def _create_amc_request():
    request = _create_header()
    max_bler_num = 180
    bler_num = random.randint(1, max_bler_num)
    request.task.amc.cell_id = bler_num % 2  # just test, make cell id randomly, configured cell id is 0 or 1
    for i in range(bler_num):
        request.task.amc.blers[i] = random.random()
    return request


def _main():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = task_service_pb2_grpc.TaskServiceStub(channel)
    create_funcs = {'grid': _create_grid_request, 'amc': _create_amc_request}
    task_request = create_funcs[FLAGS.model_name]()
    print('Create task request is: \n{}\n'.format(task_request))
    start = time.time()
    response = stub.create(task_request)
    end = time.time()
    print('Task response is: \n{}'.format(response))
    print('Running Time: {}s'.format(end - start))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=True,
                        help='Enable verbose output')
    parser.add_argument('-m', '--model-name', type=str, required=False, default="amc", choices=['grid', "amc"],
                        help='Name of model')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Adlik serving server URL. Default is localhost:8500.')
    parser.add_argument('-s', '--is-sync', type=bool, required=False, default=True,
                        help='Whether run task synchronously, wait result until task is done if synchronous. '
                             'Default is True.')
    parser.add_argument('-i', '--input', type=str, required=False,
                        default="",
                        help='File path of input csv, required for grid algorithm.')
    parser.add_argument('-o', '--output', type=str, required=False,
                        default="",
                        help='File path of output csv, required for grid algorithm.')

    FLAGS = parser.parse_args()
    _main()
