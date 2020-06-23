FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        https://apt.repos.intel.com/openvino/2019/GPG-PUB-KEY-INTEL-OPENVINO-2019 \
        "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://apt.repos.intel.com/openvino/2019 all main\n\
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
        cuda-cufft-dev-10-2 \
        cuda-cupti-dev-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        default-jdk-headless \
        flake8 \
        git \
        "intel-openvino-ie-rt-core-$ID-$VERSION_CODENAME-2019.3.344" \
        iwyu \
        libclang-common-5.0-dev \
        libclang-common-6.0-dev \
        'libcublas-dev=10.2.*' \
        'libcudnn7=*+cuda10.2' \
        'libcudnn7-dev=*+cuda10.2' \
        'libnvinfer7=7.0.*+cuda10.2' \
        'libnvinfer-dev=7.0.*+cuda10.2' \
        'libnvinfer-plugin7=7.0.*+cuda10.2' \
        'libnvonnxparsers7=7.0.*+cuda10.2' \
        'libnvonnxparsers-dev=7.0.*+cuda10.2' \
        'libnvparsers7=7.0.*+cuda10.2' \
        libtool \
        protobuf-compiler \
        python3-pip \
        python3-six \
        python3-wheel && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcublas10 libcublas-dev libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev

RUN python3 -c 'import shutil, sys, urllib.request; shutil.copyfileobj(urllib.request.urlopen(sys.argv[1]), sys.stdout.buffer)' \
        https://github.com/bazelbuild/buildtools/releases/download/3.2.1/buildifier > /usr/local/bin/buildifier && \
    chmod +x /usr/local/bin/buildifier
