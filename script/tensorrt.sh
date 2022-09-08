#!/usr/bin/env bash

if [[ ${CUDA_VERSION} = '10.0' ]]; then
  apt-get update && \
  apt-get install --no-install-recommends -y \
      cuda-cupti-dev-10-2 \
      cuda-cufft-dev-10-0 \
      cuda-curand-dev-10-0 \
      cuda-cusolver-dev-10-0 \
      cuda-cusparse-dev-10-0 \
      cuda-nvrtc-10-0 \
      cuda-cudart-dev-10-0 \
      cuda-compat-10-0
elif [[ ${CUDA_VERSION} = '10.2' ]]; then
  apt-get update && \
  apt-get install --no-install-recommends -y \
      cuda-cupti-dev-10-2 \
      cuda-cufft-dev-10-2 \
      cuda-curand-dev-10-2 \
      cuda-cusolver-dev-10-2 \
      cuda-cusparse-dev-10-2 \
      cuda-nvrtc-10-2 \
      cuda-cudart-dev-10-2 \
      cuda-compat-10-2
elif [[ ${CUDA_VERSION} = '11.0' ]]; then
  apt-get update && \
  apt-get install --no-install-recommends -y \
      cuda-command-line-tools-11-0 \
      cuda-nvrtc-dev-11-0 \
      cuda-cudart-dev-11-0 \
      libcufft-dev-11-0 \
      libcurand-dev-11-0 \
      libcusolver-dev-11-0 \
      libcusparse-dev-11-0 \
      cuda-compat-11-0
elif [[ ${CUDA_VERSION} = '11.6' ]]; then
  apt-get update && \
  apt-get install --no-install-recommends -y \
      cuda-cupti-dev-11-6 \
      cuda-nvml-dev-11-6 \
      cuda-nvrtc-dev-11-6 \
      libcublas-11-6 \
      libcublas-dev-11-6 \
      libcudnn8=*+cuda11.6 \
      libcudnn8-dev=*+cuda11.6 \
      libcufft-dev-11-6 \
      libcurand-dev-11-6 \
      libcusolver-dev-11-6 \
      libcusparse-dev-11-6 \
      libnvinfer8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.6 \
      libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda11.6
fi

if [[ ${TENSORRT_VERSION} = 7.0.* ]] ; then
  apt-get install --no-install-recommends -y \
    libcudnn7=*+cuda${CUDA_VERSION} \
    libcudnn7-dev=*+cuda${CUDA_VERSION} \
    libnvinfer7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvinfer-dev=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvinfer-plugin7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvparsers7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvonnxparsers7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    python3-libnvinfer=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION}
elif [[ ${TENSORRT_VERSION} = 7.1.* ]] || [[ ${TENSORRT_VERSION} = 7.2.* ]]; then
  apt-get install --no-install-recommends -y \
    libcudnn8=*+cuda${CUDA_VERSION} \
    libcudnn8-dev=*+cuda${CUDA_VERSION} \
    libnvinfer7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvinfer-dev=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvinfer-plugin7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvparsers7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvonnxparsers7=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION} \
    python3-libnvinfer=${TENSORRT_VERSION}-1+cuda${CUDA_VERSION}
elif [[ ${TENSORRT_VERSION} = 8.2.* ]] && [[ ${CUDA_VERSION} = '10.2' ]] ; then
  apt-get install --no-install-recommends -y \
    libcudnn8=*+cuda${CUDA_VERSION} \
    libcudnn8-dev=*+cuda${CUDA_VERSION} \
    libnvinfer8=${TENSORRT_VERSION}-1+cuda10.2 \
    libnvinfer-dev=${TENSORRT_VERSION}-1+cuda10.2 \
    libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda10.2 \
    libnvparsers8=${TENSORRT_VERSION}-1+cuda10.2 \
    libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda10.2 \
    libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda10.2 \
    python3-libnvinfer=${TENSORRT_VERSION}-1+cuda10.2
elif [[ ${TENSORRT_VERSION} = 8.2.* ]] && [[ ${CUDA_VERSION} = '11.0' ]] ; then
  apt-get install --no-install-recommends -y \
    libcudnn8=*+cuda${CUDA_VERSION} \
    libcudnn8-dev=*+cuda${CUDA_VERSION} \
    libnvinfer8=${TENSORRT_VERSION}-1+cuda11.4\
    libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.4 \
    libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda11.4 \
    libnvparsers8=${TENSORRT_VERSION}-1+cuda11.4 \
    libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda11.4 \
    libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.4 \
    python3-libnvinfer=${TENSORRT_VERSION}-1+cuda11.4
elif [[ ${TENSORRT_VERSION} = 8.4.* ]] && [[ ${CUDA_VERSION} = '11.6' ]] ; then
  apt-get install --no-install-recommends -y \
    libcudnn8=*+cuda${CUDA_VERSION} \
    libcudnn8-dev=*+cuda${CUDA_VERSION} \
    libnvinfer8=${TENSORRT_VERSION}-1+cuda11.6\
    libnvinfer-dev=${TENSORRT_VERSION}-1+cuda11.6 \
    libnvinfer-plugin8=${TENSORRT_VERSION}-1+cuda11.6 \
    libnvparsers8=${TENSORRT_VERSION}-1+cuda11.6 \
    libnvonnxparsers8=${TENSORRT_VERSION}-1+cuda11.6 \
    libnvonnxparsers-dev=${TENSORRT_VERSION}-1+cuda11.6 \
    python3-libnvinfer=${TENSORRT_VERSION}-1+cuda11.6
else
  echo "Not support this CUDA version and TensorRT version"
fi

if [[ ${TENSORRT_VERSION} = 7.0.* ]] ; then
  apt-mark hold libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 7.1.* ]] || [[ ${TENSORRT_VERSION} = 7.2.* ]] ; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 8.2.* ]]; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer8 libnvinfer-dev libnvonnxparsers8 libnvonnxparsers-dev
elif [[ ${TENSORRT_VERSION} = 8.4.* ]]; then
  apt-mark hold libcudnn8 libcudnn8-dev libnvinfer8 libnvinfer-dev libnvonnxparsers8 libnvonnxparsers-dev
fi