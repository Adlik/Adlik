# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a script for converting the .pth file to .onnx file.
"""

import torch
import os
from recbole.quick_start import load_data_and_model

base_dir = os.path.dirname(__file__)
path = base_dir + '/model'
path_list = os.listdir(path)
model_pth = ''
for filename in path_list:
    if os.path.splitext(filename)[1] == '.pth':
        model_pth = filename
        break

config, model, dataset, train_data, valid_data, test_data = load_data_and_model(
    model_file='./model/{}'.format(model_pth),
)
device = torch.device("cpu")
dummy_input = torch.randint(0, 1, (944,))
save_path = "./model/ENMF.onnx"
input_names = ["input"]
output_names = ["output"]
torch.onnx.export(model, dummy_input, save_path, input_names=input_names,
                  output_names=output_names)
