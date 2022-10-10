#!/usr/bin/env bash

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
        libcudnn7=*+cuda10.0 \
        libcudnn7-dev=*+cuda10.0 \
        libnvinfer7=${TENSORRT_VERSION}-1+cuda10.0 \
        libnvinfer-dev=${TENSORRT_VERSION}-1+cuda10.0 \
        libnvonnxparsers7=${TENSORRT_VERSION}-1+cuda10.0 \
        libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda10.0
elif [[ ${CUDA_VERSION} = '10.2' ]]; then
  apt-get update && \
    apt-get install --no-install-recommends -y \
      cuda-cufft-dev-10-2 \
      cuda-cupti-dev-10-2 \
      cuda-curand-dev-10-2 \
      cuda-cusolver-dev-10-2 \
      cuda-cusparse-dev-10-2 \
      cuda-nvml-dev-10-2 \
      cuda-nvrtc-10-2 \
      libcublas-dev=10.2.*
  if [[ ${TENSORRT_VERSION} = 7.0.* ]]; then
    apt-get update && \
      apt-get install --no-install-recommends -y \
        libcudnn7=*+cuda10.2 \
        libcudnn7-dev=*+cuda10.2
  else
    apt-get update && \
      apt-get install --no-install-recommends -y \
        'libcudnn8=*+cuda10.2' \
        'libcudnn8-dev=*+cuda10.2'
  fi
  if [[ ${TENSORRT_VERSION} = 7.*.* ]]; then
    apt-get update && \
      apt-get install --no-install-recommends -y \
        libnvinfer7=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvinfer-dev=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvinfer-plugin7=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvparsers7=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvonnxparsers7=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda10.2
  elif [[ ${TENSORRT_VERSION} = 8.*.* ]]; then
    apt-get update && \
      apt-get install --no-install-recommends -y \
        libnvinfer8=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvinfer-dev=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvparsers8=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda10.2 \
        libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda10.2
  fi
elif [[ ${CUDA_VERSION} = '11.0' ]]; then
  apt-get update && \
    apt-get install --no-install-recommends -y \
      cuda-cupti-dev-11-0 \
      cuda-nvml-dev-11-0 \
      cuda-nvrtc-dev-11-0 \
      libcublas-dev-11-0 \
      libcudnn8=*+cuda11.0 \
      libcudnn8-dev=*+cuda11.0 \
      libcufft-dev-11-0 \
      libcurand-dev-11-0 \
      libcusolver-dev-11-0 \
      libcusparse-dev-11-0
  if [[ ${TENSORRT_VERSION} = 7.*.* ]]; then
    apt-get update && \
      apt-get install --no-install-recommends -y \
        libnvinfer7=${TENSORRT_VERSION}-1+cuda11.0 \
        libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.0 \
        libnvonnxparsers7=${TENSORRT_VERSION}-1+cuda11.0 \
        libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.0
  elif [[ ${TENSORRT_VERSION} = 8.*.* ]]; then
    apt-get update && \
      apt-get install --no-install-recommends -y \
        libnvinfer8=${TENSORRT_VERSION}-1+cuda11.4 \
        libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.4 \
        libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda11.4 \
        libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.4
  fi
elif [[ ${CUDA_VERSION} = '11.6' ]]; then
  apt-get update && \
    apt-get install --no-install-recommends -y \
      cuda-command-line-tools-11-6 \
      cuda-nvrtc-dev-11-6 \
      cuda-cudart-dev-11-6 \
      cuda-nvml-dev-11-6 \
      libcufft-dev-11-6 \
      libcurand-dev-11-6 \
      libcusolver-dev-11-6 \
      libcusparse-dev-11-6 \
      libcublas-11-6 \
      libcublas-dev-11-6
  if [[ ${TENSORRT_VERSION} = 8.4.* ]] ; then
  apt-get update && \
    apt-get install --no-install-recommends -y \
      libnvinfer8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvparsers8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.6 \
      python3-libnvinfer=${TENSORRT_VERSION}-1+cuda11.6
  else
    apt-get install --no-install-recommends -y \
      libcudnn8=*+cuda${CUDA_VERSION} \
      libcudnn8-dev=*+cuda${CUDA_VERSION}
  fi
fi

if [[ ${TENSORRT_VERSION} = 7.0.* ]]; then
  apt-mark hold libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 7.1.* ]] || [[ ${TENSORRT_VERSION} = 7.2.* ]] ; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 8.2.* ]] ; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer8 libnvinfer-dev libnvonnxparsers8 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 8.4.* ]] ; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer8 libnvinfer-dev libnvonnxparsers8 libnvonnxparsers-dev
fi