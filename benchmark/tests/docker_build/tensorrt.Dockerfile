FROM ubuntu:bionic
RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        make \
        patch \
        cuda-cublas-dev-10-2 \
        cuda-cufft-dev-10-2 \
        cuda-cupti-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        git \
        libtbb2 \
        'libcudnn7=*+cuda10.2' \
        'libcudnn7-dev=*+cuda10.2' \
        'libnvinfer7=*+cuda10.2' \
        'libnvinfer-dev=*+cuda10.2' \
        'libnvonnxparsers7=*+cuda10.2' \
        'libnvonnxparsers-dev=*+cuda10.2' \
        libtool \
        python \
        python3-setuptools \
        python3-wheel \
        python3.6-dev \
        python3-six \
        python3-pip \
        protobuf-compiler && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev

RUN useradd -m john

USER john

WORKDIR /home/john

RUN bazel version

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

