ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base

RUN apt-get update && \
    apt-get install --no-install-recommends -y python3-pip && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN python3 -m pip install -U pip setuptools wheel

# Builder.

FROM base as builder

RUN apt-get update
RUN apt-get install --no-install-recommends -y protobuf-compiler

COPY model_compiler_2 /src

WORKDIR /src

RUN python3 setup.py bdist_wheel

# Runtime.

FROM base

ARG OPENVINO_VERSION

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y wget && \
    wget 'https://apt.repos.intel.com/openvino/2021/GPG-PUB-KEY-INTEL-OPENVINO-2021' -O /etc/apt/trusted.gpg.d/openvino.asc && \
    wget "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" -O /etc/apt/trusted.gpg.d/cuda.asc && \
    apt-get autoremove --purge -y wget && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://apt.repos.intel.com/openvino/2021 all main\n\
deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /" >> /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        intel-openvino-model-optimizer-"$OPENVINO_VERSION" \
        libnvinfer-plugin7='7.2.*+cuda11.0' \
        libnvinfer7='7.2.*+cuda11.0' \
        libnvonnxparsers7='7.2.*+cuda11.0' \
        libnvparsers7='7.2.*+cuda11.0' \
        python3-libnvinfer='7.2.*+cuda11.0' && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcudnn8 libnvinfer7 libnvinfer-plugin7 libnvonnxparsers7 libnvparsers7 python3-libnvinfer

COPY --from=builder /src/dist/*.whl /tmp/model-compiler-package/

RUN python3 -m pip install /tmp/model-compiler-package/*.whl && \
    rm -r /tmp/model-compiler-package ~/.cache/pip

ENV INTEL_CVSDK_DIR=/opt/intel/openvino_$OPENVINO_VERSION

CMD ["model-compiler"]
