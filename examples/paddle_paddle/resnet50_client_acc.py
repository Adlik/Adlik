# Copyright 2021 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
import os
from adlik_serving import PredictContext
import numpy as np
import cv2
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

    return input_.name, output.name


def _gen_input_data():
    if os.path.isdir(FLAGS.image_filename):
        file_names = [os.path.join(FLAGS.image_filename, f)
                      for f in os.listdir(FLAGS.image_filename)
                      if os.path.isfile(os.path.join(FLAGS.image_filename, f))]
    else:
        file_names = [FLAGS.image_filename]

    file_names.sort()

    image_data = []
    for filename in file_names:
        array = _preprocess(filename)
        image_data.append(array)
    
    true_labels = None
    if FLAGS.true_label is not None:
        with open(FLAGS.true_label, 'r') as fid:
            true_label_lines = fid.read().replace('\r', '').split('\n')
            true_labels = {}
            for line in true_label_lines:
                if len(line) and ' ' in line: 
                    key_val = line.split(' ')
                    true_labels[key_val[0]] = int(key_val[1])

    return file_names, image_data, true_labels


def _preprocess(image_path):
    def center_crop(im, crop_size):
        height, width, channel = im.shape
        w_start = (width - crop_size) // 2
        h_start = (height - crop_size) // 2
        w_end = w_start + crop_size
        h_end = h_start + crop_size
        return im[h_start:h_end, w_start:w_end, :]

    def normalize(im, mean, std):
        mean = np.array(mean)[np.newaxis, np.newaxis, :]
        std = np.array(std)[np.newaxis, np.newaxis, :]
        im = im / 255.0
        im -= mean
        im /= std
        return im

    im = cv2.imread(image_path).astype('float32')
    im = cv2.cvtColor(im, cv2.COLOR_BGR2RGB)
    im = cv2.resize(im, (256, 256), interpolation=cv2.INTER_LINEAR)
    im = center_crop(im, 224)
    im = normalize(im, [0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
    im = im.transpose((2, 0, 1))
    return im


def _postprocess(results, file_names, batch_size, labels, true_labels=None):
    if len(results.tensor) != len(file_names):
        raise Exception("expected {} results, got {}".format(batch_size, len(results)))
    if len(file_names) != batch_size:
        raise Exception("expected {} file names, got {}".format(batch_size, len(file_names)))

    acc_num = 0

    if results.batch_classes:
        for i in range(batch_size):
            j_predict = -1
            for j in range(len(results.batch_classes[i])):
                label_id = results.batch_classes[i][j]["idx"]
                results.batch_classes[i][j]["label"] = labels[label_id]
                if j_predict == -1:
                    j_predict = j
                elif results.batch_classes[i][j]["score"] > results.batch_classes[i][j_predict]["score"]:
                    j_predict = j
            if true_labels is not None:
                file_name = os.path.split(file_names[i])[-1]
                if results.batch_classes[i][j_predict]["idx"] == true_labels[file_name]:
                    acc_num += 1
            print("Image: '{}', result: {}".format(file_names[i],
                  results.batch_classes[i]))
    
    return acc_num


def _main():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    model_config = context.model_config

    input_name, output_name = _parse_model(model_config, FLAGS.model_name, FLAGS.batch_size)

    file_names, image_data, true_labels = _gen_input_data()

    cur_idx = 0
    num_of_images = len(image_data)

    labels = open(FLAGS.label_file).read().strip().split('\n')

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs = image_data[cur_idx:cur_idx + batch_size]
            outputs = file_names[cur_idx:cur_idx + batch_size]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs, next_outputs = _next_batch(batch_size - (num_of_images - image_idx))
            inputs = image_data[image_idx:] + next_inputs
            outputs = file_names[image_idx:] + next_outputs

        return inputs, outputs

    num_of_batches = num_of_images // FLAGS.batch_size
    if num_of_images % FLAGS.batch_size != 0:
        num_of_batches += 1

    acc_num = 0

    for _ in range(num_of_batches):
        i_inputs, i_outputs = _next_batch(FLAGS.batch_size)
        result = context.run(inputs={input_name: i_inputs},
                             outputs={output_name: FLAGS.classes},
                             batch_size=FLAGS.batch_size)
        acc_num += _postprocess(result[output_name], i_outputs, FLAGS.batch_size, labels, true_labels)

    if true_labels is not None:
        print("ACC: {} of {} is {}%".format(acc_num, num_of_images, acc_num/num_of_images*100))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='resnet50',
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
    parser.add_argument('-t', '--true-label', type=str, required=False, default=None,
                        help='Filepath of the true label')
    parser.add_argument('image_filename', type=str, nargs='?',
                        help='Input image.')
    parser.add_argument('label_file', type=str, nargs='?',
                        help='Label file path.')

    FLAGS = parser.parse_args()
    _main()
