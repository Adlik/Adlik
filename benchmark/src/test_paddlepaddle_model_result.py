# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
The test result of adlik performance
"""
import argparse


def _speed_of_client(client_log_path, batch_size):
    with open(client_log_path, 'r') as file:
        lines = file.readlines()
        sum_time = []
        for line in lines:
            line = line.strip('\n')
            time = line.split(' use ')[-1].split(' ')[0]
            time = float(time.strip(' '))
            sum_time.append(time)
        sum_time.pop(0)
        sum_time.pop(0)
        batch_num = len(sum_time)
        speed_processing_picture = (batch_num * batch_size) / sum(sum_time)
    return speed_processing_picture, batch_num


def _speed_of_serving(serving_log_path, batch_size):
    with open(serving_log_path, 'r') as file:
        lines = file.readlines()
        runtime = lines[0].partition('found runtime ')[-1]
        lines = [line.partition('PredictServiceImpl')[-1] for line in lines]
        sum_time = []
        for line in lines:
            if line:
                line = line.strip('\n')
                time = line.partition('time (milliseconds):')[-1]
                time = float(time.strip(' '))
                sum_time.append(time)
        sum_time.pop(0)
        sum_time.pop(0)
        batch_num = len(sum_time)
        speed_processing_picture = (batch_num * batch_size) / sum(sum_time)
    return speed_processing_picture, batch_num, runtime


def main(args):
    """
    Analyze inference results
    """
    speed_processing_picture_client, batch_num = _speed_of_client(args.client_log_path, args.batch_size)
    speed_processing_picture_serving, batch_num1, serving_runtime = _speed_of_serving(args.serving_log_path,
                                                                                      args.batch_size)
    assert batch_num == batch_num1
    if args.runtime:
        serving_runtime = args.runtime
    else:
        serving_runtime = serving_runtime
    tail_latency = speed_processing_picture_client - speed_processing_picture_serving
    print(f'Model: {args.model_name}, Runtime: {serving_runtime}')
    print(f'The time of processing one picture in the client is : {speed_processing_picture_client}ms')
    print(f'The time of processing one picture in the serving is : {speed_processing_picture_serving}ms')
    print(f'The tail latency of one picture is : {tail_latency}')


if __name__ == '__main__':
    ARGS_PARSER = argparse.ArgumentParser()
    ARGS_PARSER.add_argument('-c', '--client-log-path', type=str, required=True,
                             help='The path of client log')
    ARGS_PARSER.add_argument('-s', '--serving-log-path', type=str, required=True,
                             help='The path of serving log')
    ARGS_PARSER.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                             help='Batch size. Default is 128.')
    ARGS_PARSER.add_argument('-m', '--model-name', type=str, required=True,
                             help='The name of model')
    ARGS_PARSER.add_argument('-r', '--runtime', type=str, required=False, default=None,
                             help='The serving type')
    PARSE_ARGS = ARGS_PARSER.parse_args()
    main(PARSE_ARGS)
