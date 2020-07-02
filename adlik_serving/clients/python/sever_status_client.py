# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample of create use grpc protocol, just for test query server status
"""

import argparse
import time

from adlik_serving.apis import server_status_service_pb2_grpc, server_status_pb2
from google.protobuf import json_format
import grpc


def _grpc_query_server_status():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = server_status_service_pb2_grpc.ServerStatusServiceStub(channel)
    status_request = server_status_pb2.ServerStatusRequest()
    print('Create status request is: \n{}\n'.format(status_request))
    start = time.time()
    response = stub.serverStatus(status_request)
    print(response)
    end = time.time()
    print('Status response is: \n{}'.format(json_format.MessageToJson(response)))
    print('Running Time: {}s'.format(end - start))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:9006',
                        help='Adlik serving server URL. Default is localhost:8500.')
    FLAGS = parser.parse_args()
    _grpc_query_server_status()
