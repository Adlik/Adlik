# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a sample for Adlik_serving prediction
"""

import argparse
import os
import json
import cv2
import torch
import torchvision.transforms.functional as F
from adlik_serving import PredictContext, model_config_pb2, tensor_dtype_to_np_dtype
import numpy as np
from pathlib import Path
import yolov5_data_process as data_process
torch.set_printoptions(profile="full")
np.set_printoptions(threshold=np.inf)


def _parse_model(config, model_name, batch_size):
    if config.max_batch_size == 0:
        if batch_size != 1:
            raise Exception("batching not supported for model '" + model_name + "'")
    else:  # max_batch_size > 0
        if batch_size > config.max_batch_size:
            raise Exception(
                "expecting batch size <= {} for model '{}'".format(config.max_batch_size, model_name))

    input_ = config.input[0]

    output_name = [output.name for output in config.output]
    if input_.format == model_config_pb2.ModelInput.FORMAT_NHWC:
        h = input_.dims[0]
        w = input_.dims[1]
        c = input_.dims[2]
    else:
        c = input_.dims[0]
        h = input_.dims[1]
        w = input_.dims[2]
    return input_.name, output_name, c, h, w, input_.format, tensor_dtype_to_np_dtype(input_.data_type)


def _gen_input_data():
    image_datas = []
    image_shapes = []
    file_names = []
    img_size = 640

    if os.path.isdir(FLAGS.image_filename):
        file_names = [os.path.join(FLAGS.image_filename, f)
                      for f in os.listdir(FLAGS.image_filename)
                      if os.path.isfile(os.path.join(FLAGS.image_filename, f))]
    else:
        file_names = [FLAGS.image_filename]

    file_names.sort()

    for filename in file_names:
        img = cv2.imread(filename)
        img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
        h1, w1 = h0, w0 = img.shape[:2]  # orig hw
        r = img_size / max(h0, w0)  # ratio
        if r != 1:
            h1, w1 = int(h0 * r), int(w0 * r)
        img, pad = data_process.letterbox(img, new_shape=(640, 640))
        img = F.to_tensor(img)
        img = np.asarray(img, dtype='float32')
        image_datas.append(img)
        shapes = (h0, w0), ((h1 / h0, w1 / w0), pad)
        image_shapes.append(shapes)

    return file_names, image_datas, image_shapes


def _main_single():
    context = PredictContext(FLAGS.model_name, url=FLAGS.url, protocol=FLAGS.protocol, verbose=True)
    model_config = context.model_config

    input_names, output_names, c, h, w, data_format, dtype = _parse_model(
        model_config, FLAGS.model_name, FLAGS.batch_size)

    file_names, image_datas, shapes = _gen_input_data()

    cur_idx = 0
    num_of_images = len(image_datas)
    class_map = data_process.coco80_to_coco91_class()
    jdict = []
    conf_thres = 0.001   # confidence threshold
    iou_thres = 0.6   # NMS IoU threshold

    def _next_batch(batch_size):
        nonlocal cur_idx
        if cur_idx + batch_size <= num_of_images:
            inputs = image_datas[cur_idx:cur_idx + batch_size]
            outputs = file_names[cur_idx:cur_idx + batch_size]
            path = file_names[cur_idx]
            shape = shapes[cur_idx]
            cur_idx = (cur_idx + batch_size) % num_of_images
        else:
            image_idx = cur_idx
            cur_idx = 0
            next_inputs, next_outputs, next_raws = _next_batch(batch_size - (num_of_images - image_idx))
            inputs = image_datas[image_idx:] + next_inputs
            outputs = file_names[image_idx:] + next_outputs

        return inputs, outputs, path, shape

    num_of_batches = num_of_images // FLAGS.batch_size
    if num_of_images % FLAGS.batch_size != 0:
        num_of_batches += 1

    for i in range(num_of_batches):
        i_inputs, i_outputs, path, shape = _next_batch(FLAGS.batch_size)
        result = context.run(inputs={input_names: i_inputs},
                             outputs=output_names,
                             batch_size=FLAGS.batch_size)

        outputs_result = [result[output_name].tensor for output_name in output_names]
        detect_heads = data_process.DetectHead()
        out, _ = detect_heads(outputs_result)
        pred = data_process.non_max_suppression(out, conf_thres, iou_thres, labels=[], multi_label=True)[0]
        data_process.scale_coords(i_inputs[0].shape[1:], pred[:, :4], shape[0], shape[1])
        path = Path(path)
        data_process.save_one_json(pred, jdict, path, class_map)

    anno_json = './instances_val2017.json'
    pred_json = './yolov5_predictions.json'
    with open(pred_json, 'w') as f:
        json.dump(jdict, f)

    try:  # https://github.com/cocodataset/cocoapi/blob/master/PythonAPI/pycocoEvalDemo.ipynb
        from pycocotools.coco import COCO
        from pycocotools.cocoeval import COCOeval

        anno = COCO(anno_json)  # init annotations api
        pred = anno.loadRes(pred_json)  # init predictions api
        eval = COCOeval(anno, pred, 'bbox')
        eval.evaluate()
        eval.accumulate()
        eval.summarize()
        map, map50 = eval.stats[:2]  # update results (mAP@0.5:0.95, mAP@0.5)

    except Exception as e:
        print(f'pycocotools unable to run: {e}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--model-name', type=str, required=False, default='yolov5s',
                        help='Name of model')
    parser.add_argument('-b', '--batch-size', type=int, required=False, default=1,
                        help='Batch size. Default is 1.')
    parser.add_argument('-c', '--classes', type=int, required=False, default=80,
                        help='Number of class results to report. Default is 1.')
    parser.add_argument('-u', '--url', type=str, required=False, default='localhost:8500',
                        help='Server URL. Default is localhost:8500.')
    parser.add_argument('-i', '--protocol', type=str, required=False, default='grpc',
                        help='Protocol ("http"/"grpc") used to ' +
                             'communicate with service. Default is "grpc".')
    parser.add_argument('image_filename', type=str, nargs='?', default='val2017',
                        help='Input image.')
    FLAGS = parser.parse_args()
    _main_single()
