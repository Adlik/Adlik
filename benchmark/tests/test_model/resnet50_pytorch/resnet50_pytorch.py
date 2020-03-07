# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
Resnet50 model
"""

from torch.autograd import Variable
import torchvision
import torch.onnx
import torch
import os


def get_model():
    return torchvision.models.resnet50(pretrained=True)


def main():
    device = torch.device('cpu')
    dummy_input = Variable(torch.randn(1, 3, 224, 224))
    dummy_input = dummy_input.to(device)
    model = get_model()
    save_path = os.path.join(os.path.dirname(__file__), 'model', 'resnet50.onnx')
    dir_name = os.path.dirname(save_path)
    os.makedirs(dir_name, exist_ok=True)
    torch.onnx.export(model, dummy_input, save_path, verbose=True, keep_initializers_as_inputs=True)


if __name__ == '__main__':
    main()
