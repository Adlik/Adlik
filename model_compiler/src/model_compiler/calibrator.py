# Copyright 2021 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import tensorrt as trt
import pycuda.driver as cuda
import pycuda.autoinit  # pylint: disable=imported but unused

import os
import numpy as np
from PIL import Image

import torchvision.transforms as transforms


def _load_calibration_data(filepath):
    txt_file_path = os.path.join(filepath, '..', 'calibration_data.txt')
    txt_file = open(txt_file_path, 'r')
    image_lines = txt_file.readlines()
    np.random.shuffle(image_lines)
    images = [os.path.join(filepath, image_line.split('\n')[0]) for image_line in image_lines]
    return images


class MyCalibrator(trt.IInt8EntropyCalibrator2):

    def __init__(self, calibration_dataset, batch_size, input_shapes):
        trt.IInt8EntropyCalibrator2.__init__(self)

        self.cache_file = None

        input_shape = input_shapes[0]
        self.batch_size = batch_size
        self.channel = sorted(input_shape[1:])[0]
        self.width = sorted(input_shape[1:])[1]
        self.height = sorted(input_shape[1:])[2]

        self.images = _load_calibration_data(calibration_dataset)
        self.current_batch = 0
        self.max_batch = len(self.images)//self.batch_size

        self.data_size = trt.volume([self.batch_size, self.channel, self.height, self.width]) * trt.float32.itemsize
        self.device_input = cuda.mem_alloc(self.data_size)

    def _next_batch(self):
        if self.current_batch < self.max_batch:
            batch_files = self.images[self.current_batch * self.batch_size: (self.current_batch + 1) * self.batch_size]
            batch_images = np.zeros((self.batch_size, self.channel, self.height, self.width), dtype=np.float32)
            normalize = transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
            img_transform = transforms.Compose([transforms.CenterCrop(self.width), transforms.ToTensor(), normalize, ])
            for i, f in enumerate(batch_files):
                img = Image.open(f)
                if len(img.split()) != 3:
                    print("Notice! {} not valid img! jump over!".format(f))
                    continue
                img = img_transform(img).numpy()
                batch_images[i] = img
            self.current_batch += 1
            print("batch:[{}/{}], containing {} images".format(self.current_batch, self.max_batch, self.batch_size))

            return np.ascontiguousarray(batch_images)
        else:
            return np.array([])

    def get_batch_size(self):
        return self.batch_size

    def get_batch(self, names, p_str=None):
        batch_imgs = self._next_batch()
        if batch_imgs.size == 0 or batch_imgs.size != self.batch_size*self.channel*self.height*self.width:
            return None
        cuda.memcpy_htod(self.device_input, batch_imgs.astype(np.float32))
        return [int(self.device_input)]

    def read_calibration_cache(self):
        # If there is a cache, use it instead of calibrating again. Otherwise, implicitly return None.
        return self.cache_file

    def write_calibration_cache(self, cache):
        self.cache_file = cache
