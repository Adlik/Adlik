# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample of create and run k-means task use grpc protocol
"""

import argparse

from adlik_serving.apis import task_pb2, task_service_pb2_grpc
import grpc

FLAGS = None


def _create_request():
    request = task_pb2.CreateTaskRequest()
    request.model_spec.name = FLAGS.model_name
    request.task_type = 1  # Training task
    request.is_sync = True
    request.algorithm = 'k-means'
    request.kmeans_task.n_clusters = FLAGS.clusters
    request.kmeans_task.input = FLAGS.input
    request.kmeans_task.max_iter = FLAGS.max_iter
    request.kmeans_task.compute_labels = True
    request.kmeans_task.lable_name = FLAGS.label_name
    request.kmeans_task.output = FLAGS.output
    return request


def _main():
    channel = grpc.insecure_channel(FLAGS.url)
    stub = task_service_pb2_grpc.TaskServiceStub(channel)
    task_request = _create_request()
    print(task_request)
    response = stub.create(task_request)
    print('******************')
    print(response)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-v', '--verbose', action="store_true", required=False, default=True,
                        help='Enable verbose output')
    parser.add_argument('-m', '--model-name', type=str, required=False, default="grid",
                        help='Name of model')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Adlik serving server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--input', type=str, required=False,  # required should be 'True'
                        default='/home/jsy/ai/code/Adlik/adlik_serving/runtime/ml/algorithm/test_data/data1.csv',
                        help='File path of input csv.')
    parser.add_argument('-o', '--output', type=str, required=False,  # required should be 'True'
                        default='/home/jsy/ai/code/Adlik/adlik_serving/runtime/ml/algorithm/test_data/result1.csv',
                        help='File path of output csv.')
    parser.add_argument('--max-iter', type=int, required=False, default=10,
                        help='Maximum iterations. Default is 1.')
    parser.add_argument('-k', '--clusters', type=int, required=False, default=3,
                        help='n clusters. Default is 3.')
    parser.add_argument('-l', '--label-name', type=str, required=False, default="y",
                        help='Output label name. Default is y.')

    FLAGS = parser.parse_args()
    _main()
