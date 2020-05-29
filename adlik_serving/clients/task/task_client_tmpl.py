# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a task client template script, just for test. User should fulfill the code marked with
"todo" depending on their own requirements.
"""

import argparse
import time

from adlik_serving.apis import task_pb2, task_service_pb2_grpc
from google.protobuf import json_format
import grpc
import requests

FLAGS = None


def _create_detail():
    pass
    # detail = xxx
    # return detail


def _create_request():
    task_request = task_pb2.CreateTaskRequest()
    task_request.model_spec.name = FLAGS.model_name
    task_request.task_type = task_pb2.TaskType.TRAINING_TASK
    task_request.timeout_seconds = 0  # Set timeout if needed

    # todo: create task detail
    # detail = _create_detail()
    # task_request.detail.Pack(detail)

    return task_request


def _grpc_visit():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = task_service_pb2_grpc.TaskServiceStub(channel)
    task_request = _create_request()
    print('Create task request is: \n{}\n'.format(json_format.MessageToJson(task_request)))

    start = time.time()
    response = stub.create(task_request)
    end = time.time()

    print('Task response is: \n{}'.format(json_format.MessageToJson(response)))
    print('Running Time: {}s'.format(end - start))


def _http_visit():
    task_request = _create_request()
    url = 'http://%s/v1/models/%s' % (FLAGS.url, FLAGS.model_name)
    start = time.time()
    response = requests.post(url + ":ml_predict",
                             data=json_format.MessageToJson(task_request, preserving_proto_field_name=True))
    end = time.time()
    response.raise_for_status()
    print(response.json())
    print('Running Time: {}s'.format(end - start))


def _main():
    if FLAGS.protocol == "http":
        _http_visit()
    else:
        _grpc_visit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=True,
                        help='Enable verbose output')
    parser.add_argument('-m', '--model-name', type=str, required=True,
                        help='Name of model')
    parser.add_argument('-p', '--protocol', type=str, required=False, default='grpc', choices=['grpc', "http"],
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Adlik serving server URL. Default is localhost:8500.')

    # todo: add other arguments if needed

    FLAGS = parser.parse_args()
    _main()
