# Copyright 2021 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
"""
This is a demo code which export PaddlePaddle ResNet50 model
"""

import os
import paddle
import paddlehub as hub


def export_paddle_resnet50(save_dir):
    ''' export PaddlePaddle model
    Args:
        save_dir(str): Dir to save exported PaddlePaddle model (.pdmodel .pdiparams .pdiparams.info)
    Returns:
        None
    '''
    model = hub.Module(name="resnet50_vd_imagenet_ssld")
    input_spec = paddle.static.InputSpec([64, 3, 224, 224], "float32", "image")
    paddle.jit.save(model,
                    path=os.path.join(save_dir, "resnet50"),
                    input_spec=[input_spec])


if __name__ == '__main__':
    save_dir = './model_resnet50'
    export_paddle_resnet50(save_dir)
