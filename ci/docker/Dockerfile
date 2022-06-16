FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
        "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://apt.repos.intel.com/openvino/2022 $VERSION_CODENAME main\n\
deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        clang-format \
        clang-tidy \
        cuda-cupti-dev-11-0 \
        cuda-nvml-dev-11-0 \
        cuda-nvrtc-11-0 \
        default-jdk-headless \
        flake8 \
        git \
        "libopenvino-dev-2022.1.0" \
        iwyu \
        libclang-common-5.0-dev \
        libclang-common-6.0-dev \
        cuda-nvprune-11-0 \
        cuda-nvtx-11-0 \
        cuda-command-line-tools-11-0 \
        cuda-compat-11-0 \
        libcublas-dev-11-0 \
        'libcudnn8=*+cuda11.0' \
        'libcudnn8-dev=*+cuda11.0' \
        libcufft-dev-11-0 \
        libcurand-dev-11-0 \
        libcusolver-dev-11-0 \
        libcusparse-dev-11-0 \
        'libnvinfer7=7.2.*+cuda11.0' \
        'libnvinfer-dev=7.2.*+cuda11.0' \
        'libnvinfer-plugin7=7.2.*+cuda11.0' \
        'libnvonnxparsers7=7.2.*+cuda11.0' \
        'libnvonnxparsers-dev=7.2.*+cuda11.0' \
        'libnvparsers7=7.2.*+cuda11.0' \
        libtool \
        protobuf-compiler \
        python3-dev \
        python3-pip \
        python3-six \
        python3-wheel && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcudnn8 libcudnn8-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev

RUN python3 -m pip install -U pip setuptools wheel

ENV PATH /usr/local/nvidia/bin:/usr/local/cuda-11.0/bin:${PATH}
ENV LD_LIBRARY_PATH=/usr/local/cuda-11.0/targets/x86_64-linux/lib:$LD_LIBRARY_PATH
ENV CPLUS_INCLUDE_PATH=/usr/include/python3.6m:$CPLUS_INCLUDE_PATH

RUN python3 -c 'import shutil, sys, urllib.request; shutil.copyfileobj(urllib.request.urlopen(sys.argv[1]), sys.stdout.buffer)' \
        https://github.com/bazelbuild/buildtools/releases/download/3.4.0/buildifier > /usr/local/bin/buildifier && \
    chmod +x /usr/local/bin/buildifier
