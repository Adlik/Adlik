# Copyright 2019 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

from .schedule_search_x86 import CpuSearch


class Compiler:
    def __init__(self, relay_mod, params, input_shape) -> None:
        self.mod = relay_mod
        self.params = params
        self.input_shape = input_shape
        self.strategy = None
        self.schedule_file = None

    def set_strategy(self, strategy):
        self.strategy = strategy

    def search_schedule(self):
        self.schedule_file = self.strategy.search(self.mod, self.params, self.input_shape)

    def compile(self):
        return self.strategy.compile(self.mod, self.params, self.schedule_file)


def compile_relay(model, params, config, shape_dict):

    compiler = Compiler(model, params, shape_dict)

    if config.target.startswith('llvm'):
        compiler.set_strategy(CpuSearch(config.target))
    elif config.target == 'arm-cpu':
        raise NotImplementedError('Arm compile not supported yet')
    elif config.target == 'cuda':
        raise NotImplementedError('CUDA compile not supported yet')
    else:
        raise NameError(f'Not supported target name: "{config.target}"')

    if config.need_search_schedule:
        compiler.search_schedule()

    return compiler.compile()
