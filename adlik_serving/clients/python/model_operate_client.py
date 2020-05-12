# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample of create use grpc protocol, just for test grid and amc case
"""

import argparse
import time

from adlik_serving.apis import model_operate_pb2, model_operate_service_pb2_grpc
from google.protobuf import json_format
import grpc
import requests

FLAGS = None


def _create_add_model_request():
    request = model_operate_pb2.ModelOperateRequest()
    request.model_name = FLAGS.model_name
    request.operate_name = FLAGS.operate_name
    request.path = FLAGS.path
    return request


def _create_delete_model_request():
    request = model_operate_pb2.ModelOperateRequest()
    request.model_name = FLAGS.model_name
    request.operate_name = FLAGS.operate_name
    return request


def _grpc_add_model():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = model_operate_service_pb2_grpc.ModelOperateServiceStub(channel)
    operate_request = _create_add_model_request()
    print('model operate request is: \n{}\n'.format(json_format.MessageToJson(operate_request)))
    start = time.time()
    response = stub.addModel(operate_request)
    end = time.time()
    print('Task response is: \n{}'.format(response.status))
    print('Running Time: {}s'.format(end - start))


def _grpc_delete_model():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = model_operate_service_pb2_grpc.ModelOperateServiceStub(channel)
    operate_request = _create_delete_model_request()
    print('model operate request is: \n{}\n'.format(json_format.MessageToJson(operate_request)))
    start = time.time()
    response = stub.deleteModel(operate_request)
    end = time.time()
    print('model operate response is: \n{}'.format(response.status))
    print('Running Time: {}s'.format(end - start))


grpc_operate = {
    'add': _grpc_add_model,
    'delete': _grpc_delete_model
}


def _grpc_visit():
    grpc_operate[FLAGS.operate_name]()


def _main():
    _grpc_visit()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=True,
                        help='Enable verbose output')
    parser.add_argument('-m', '--model-name', type=str, required=False, default="mm",
                        help='Name of model')
    parser.add_argument('-p', '--protocol', type=str, required=False, default='grpc', choices=['grpc', "http"],
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:9006',
                        help='Adlik serving server URL. Default is localhost:8500.')
    parser.add_argument('-e', '--operate-name', type=str, required=False,
                        default='add', help='model operate name')
    parser.add_argument('-a', '--path', type=str, required=False,
                        default='/home/john/mm',
                        help='operate model path')

    FLAGS = parser.parse_args()
    _main()
