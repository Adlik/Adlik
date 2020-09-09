# Copyright 2020 ZTE corporation. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

import ctypes
from ctypes import CDLL, c_char_p, c_int, c_size_t, c_uint, c_void_p, util

# CUDA types.

_CuResult = c_int
_CuDevice = c_int
_CuContext = c_void_p
_CuDevicePtr = c_size_t

_CUDA_DLL = CDLL(util.find_library('cuda'))

# Error handling.

_CU_GET_ERROR_STRING = _CUDA_DLL.cuGetErrorString

# Initialization.

_CU_INIT = _CUDA_DLL.cuInit

# Device management.

_CU_DEVICE_GET = _CUDA_DLL.cuDeviceGet

# Context management.

_CU_CTX_CREATE = _CUDA_DLL.cuCtxCreate_v2
_CU_CTX_DESTROY = _CUDA_DLL.cuCtxDestroy_v2

# Memory management.

_CU_MEM_ALLOC = _CUDA_DLL.cuMemAlloc_v2
_CU_MEM_FREE = _CUDA_DLL.cuMemFree_v2


def _check_cuda_error(result, _func, _arguments):
    if result != 0:
        error_name = c_char_p()

        _CU_GET_ERROR_STRING(result, ctypes.byref(error_name))

        raise RuntimeError(f'CUDA error: {error_name.value.decode()} ({result}).')

    return result


for func, arg_types in [(_CU_GET_ERROR_STRING, [_CuResult, ctypes.POINTER(c_char_p)]),
                        (_CU_INIT, [c_uint]),
                        (_CU_DEVICE_GET, [ctypes.POINTER(_CuDevice), c_int]),
                        (_CU_CTX_CREATE, [ctypes.POINTER(_CuContext), c_uint, _CuDevice]),
                        (_CU_CTX_DESTROY, [_CuContext]),
                        (_CU_MEM_ALLOC, [ctypes.POINTER(_CuDevicePtr), c_size_t]),
                        (_CU_MEM_FREE, [_CuDevicePtr])]:
    setattr(func, 'argtypes', arg_types)
    setattr(func, 'errcheck', _check_cuda_error)


class _Context:
    def __init__(self, handle: _CuContext):
        self._handle = handle

    def __del__(self):
        self.close()

    def close(self):
        if self._handle is not None:
            handle = self._handle
            self._handle = None

            _CU_CTX_DESTROY(handle)


class _Device:
    def __init__(self, handle: _CuDevice):
        self._handle = handle

    def create_context(self, flags) -> _Context:
        context_handle = _CuContext()

        _CU_CTX_CREATE(ctypes.byref(context_handle), flags, self._handle)

        return _Context(context_handle)

    @staticmethod
    def get(ordinal):
        handle = _CuDevice()

        _CU_DEVICE_GET(ctypes.byref(handle), ordinal)

        return _Device(handle)


class _Memory:
    def __init__(self, handle: _CuDevicePtr):
        self._handle = handle

    def __del__(self):
        self.close()

    def __int__(self):
        return self._handle.value

    def close(self):
        if self._handle is not None:
            handle = self._handle
            self._handle = None

            _CU_MEM_FREE(handle)

    @staticmethod
    def allocate(size):
        handle = _CuDevicePtr()

        _CU_MEM_ALLOC(ctypes.byref(handle), size)

        return _Memory(handle)


def init():
    _CU_INIT(0)


def get_device(ordinal):
    return _Device.get(ordinal=ordinal)


def allocate_memory(size):
    return _Memory.allocate(size=size)
