# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

"""
This is a script for training recommend model.
Please refer to https://recbole.io/docs/index.html
"""
from recbole.quick_start import run_recbole


def main():
    parameter_dict = {
        'neg_sampling': None,
        'checkpoint_dir': './model'
    }
    run_recbole(model='ENMF', dataset='ml-100k', config_dict=parameter_dict)


if __name__ == '__main__':
    main()
