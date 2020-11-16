#!/usr/bin/env bash

if [[ ${TENSORRT_VERSION} = 7.0.* ]]; then
  if [[ ${CUDA_VERSION} = '10.0' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cublas-dev-10-0 \
        cuda-cufft-dev-10-0 \
        cuda-cupti-10-0 \
        cuda-curand-dev-10-0 \
        cuda-cusolver-dev-10-0 \
        cuda-cusparse-dev-10-0 \
        cuda-nvml-dev-10-0 \
        cuda-nvrtc-10-0 \
        'libcudnn7=*+cuda10.0' \
        'libcudnn7-dev=*+cuda10.0' \
        'libnvinfer7=7.0.*+cuda10.0' \
        'libnvinfer-dev=7.0.*+cuda10.0' \
        'libnvonnxparsers7=7.0*+cuda10.0' \
        'libnvonnxparsers-dev=7.0.*+cuda10.0'
  elif [[ ${CUDA_VERSION} = '10.2' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cufft-10-2 \
        cuda-cufft-dev-10-2 \
        cuda-cupti-dev-10-2 \
        cuda-curand-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-cusparse-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        libcublas-dev=10.2.* \
        'libcudnn7=*+cuda10.2' \
        'libcudnn7-dev=*+cuda10.2' \
        'libnvinfer7=7.0.*+cuda10.2' \
        'libnvinfer-dev=7.0.*+cuda10.2' \
        'libnvinfer-plugin7=7.0.*+cuda10.2' \
        'libnvparsers7=7.0.*+cuda10.2' \
        'libnvonnxparsers7=7.0.*+cuda10.2' \
        'libnvonnxparsers-dev=7.0.*+cuda10.2'

  apt-mark hold libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
  fi
elif [[ ${TENSORRT_VERSION} = 7.1.* ]]; then
  if [[ ${CUDA_VERSION} = '10.2' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cufft-10-2 \
        cuda-cufft-dev-10-2 \
        cuda-cupti-dev-10-2 \
        cuda-curand-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-cusparse-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        libcublas-dev=10.2.* \
        'libcudnn8=*+cuda10.2' \
        'libcudnn8-dev=*+cuda10.2' \
        'libnvinfer7=7.1.*+cuda10.2' \
        'libnvinfer-dev=7.1.*+cuda10.2' \
        'libnvinfer-plugin7=7.1.*+cuda10.2' \
        'libnvparsers7=7.1.*+cuda10.2' \
        'libnvonnxparsers7=7.1.*+cuda10.2' \
        'libnvonnxparsers-dev=7.1.*+cuda10.2'
  elif [[ ${CUDA_VERSION} = '11.0' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cupti-dev-11-0 \
        cuda-nvml-dev-11-0 \
        cuda-nvrtc-11-0 \
        libcublas-dev-11-0 \
        libcudnn8=*+cuda11.0 \
        libcudnn8-dev=*+cuda11.0 \
        libcufft-dev-11-0 \
        libcurand-dev-11-0 \
        libcusolver-dev-11-0 \
        libcusparse-dev-11-0 \
        libnvinfer7=7.1.*+cuda11.0 \
        libnvinfer-dev=7.1.*+cuda11.0 \
        libnvonnxparsers7=7.1.*+cuda11.0 \
        libnvonnxparsers-dev=7.1.*+cuda11.0

  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
  fi
elif [[ ${TENSORRT_VERSION} = 7.2.0.* ]]; then
  if [[ ${CUDA_VERSION} = '11.0' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
      cuda-cupti-dev-11-0 \
      cuda-nvml-dev-11-0 \
      cuda-nvrtc-11-0 \
      libcublas-dev-11-0 \
      libcudnn8=*+cuda11.0 \
      libcudnn8-dev=*+cuda11.0 \
      libcufft-dev-11-0 \
      libcurand-dev-11-0 \
      libcusolver-dev-11-0 \
      libcusparse-dev-11-0 \
      libnvinfer7=7.2.0-1+cuda11.0 \
      libnvinfer-dev=7.2.0-1+cuda11.0 \
      libnvonnxparsers7=7.2.0-1+cuda11.0 \
      libnvonnxparsers-dev=7.2.0-1+cuda11.0
    apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
  else
    echo "CUDA version and TensorRT version do not match"
  fi
elif [[ ${TENSORRT_VERSION} = 7.2.1.* ]]; then
  if [[ ${CUDA_VERSION} = '10.2' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cufft-10-2 \
        cuda-cufft-dev-10-2 \
        cuda-cupti-dev-10-2 \
        cuda-curand-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-cusparse-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        libcublas-dev=10.2.* \
        'libcudnn8=*+cuda10.2' \
        'libcudnn8-dev=*+cuda10.2' \
        'libnvinfer7=7.2.*+cuda10.2' \
        'libnvinfer-dev=7.2.*+cuda10.2' \
        'libnvinfer-plugin7=7.2.*+cuda10.2' \
        'libnvparsers7=7.2.*+cuda10.2' \
        'libnvonnxparsers7=7.2.*+cuda10.2' \
        'libnvonnxparsers-dev=7.2.*+cuda10.2'
  elif [[ ${CUDA_VERSION} = '11.0' ]]; then
    apt-get update && \
    apt-get install --no-install-recommends -y \
      cuda-cupti-dev-11-0 \
      cuda-nvml-dev-11-0 \
      cuda-nvrtc-11-0 \
      libcublas-dev-11-0 \
      libcudnn8=*+cuda11.0 \
      libcudnn8-dev=*+cuda11.0 \
      libcufft-dev-11-0 \
      libcurand-dev-11-0 \
      libcusolver-dev-11-0 \
      libcusparse-dev-11-0 \
      libnvinfer7=7.2.*+cuda11.0 \
      libnvinfer-dev=7.2.*+cuda11.0 \
      libnvonnxparsers7=7.2.*+cuda11.0 \
      libnvonnxparsers-dev=7.2.*+cuda11.0
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
  fi
else
  echo "Not support this CUDA version and TensorRT version"
fi









