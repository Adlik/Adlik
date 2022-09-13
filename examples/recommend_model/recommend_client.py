# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for recommend prediction
"""

import argparse
from adlik_serving import PredictContext, tensor_dtype_to_np_dtype
import torch
from recbole.config import Config
from recbole.data import create_dataset

FLAGS = None


def _parse_model(config, model_name, batch_size):
    if config.max_batch_size == 0:
        if batch_size != 1:
            raise Exception("batching not supported for model '" + model_name + "'")
    else:  # max_batch_size > 0
        if batch_size > config.max_batch_size:
            raise Exception(
                "expecting batch size <= {} for model '{}'".format(config.max_batch_size, model_name))
    input_ = config.input[0]
    output = config.output[0]
    return input_.name, output.name, input_.format, tensor_dtype_to_np_dtype(input_.data_type)


def _preprocess():
    config_dataset = Config(model=FLAGS.model_name, dataset=FLAGS.dataset)
    dataset = create_dataset(config_dataset)
    # user_id: '196' -> 1  '186' -> 2   '62' -> 10
    uid_series = dataset.token2id(dataset.uid_field, [FLAGS.user_id])
    return uid_series, dataset


def _postprocess(scores):
    scores = scores.tensor
    scores = torch.from_numpy(scores)
    topk_score, topk_iid_list = torch.topk(scores, 10)
    return topk_score, topk_iid_list


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    model_config = context.model_config
    input_name, output_name, data_format, dtype = _parse_model(model_config, FLAGS.model_name, FLAGS.batch_size)
    uid_series, dataset = _preprocess()

    result = context.run(inputs={input_name: uid_series},
                         outputs=[output_name],
                         batch_size=FLAGS.batch_size)

    # Get the top ranked item for each user¶
    topk_score, topk_iid_list = _postprocess(result[output_name])
    external_item_list = dataset.id2token(dataset.iid_field, topk_iid_list.cpu())

    print("topk_score: ", topk_score)  # scores of top 10 items
    print("topk_iid_list: ", topk_iid_list)  # internal id of top 10 items
    print("external_item_list: ", external_item_list)  # external tokens of top 10 items


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=True, default='ENMF',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-c', '--classes', type=int, required=False, default=1,
                        help='Number of class results to report. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('-UID', '--user_id', type=str, required=True, default='196',
                        help='External_user_id.')
    parser.add_argument('-d', '--dataset', type=str, required=False, default='ml-100k',
                        help='Dataset. Default is "ml-100k".')
    FLAGS = parser.parse_args()
    _main()
