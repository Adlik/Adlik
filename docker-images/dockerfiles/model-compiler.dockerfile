ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base
COPY script/run_compiler.sh /script/run_compiler.sh
COPY script/tensorrt.sh /script/tensorrt.sh
RUN apt-get update && \
    apt-get install --no-install-recommends -y python3-pip && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN python3 -m pip install -U pip setuptools wheel

# Builder.

FROM base as builder

RUN apt-get update
RUN apt-get install --no-install-recommends -y protobuf-compiler

COPY model_compiler /src

WORKDIR /src

RUN python3 setup.py bdist_wheel

# Runtime.

FROM base

ARG OPENVINO_VERSION
ARG TENSORRT_VERSION
ARG CUDA_VERSION
ENV TENSORRT_VERSION=${TENSORRT_VERSION}
ENV CUDA_VERSION=${CUDA_VERSION}

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

RUN chmod +x /script/tensorrt.sh
RUN /script/tensorrt.sh

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        intel-openvino-model-optimizer-"$OPENVINO_VERSION" && \
    apt-get clean && \
    find /var/lib/apt/lists -delete &&\
    ln -s /usr/local/cuda-"$CUDA_VERSION" /usr/local/cuda

RUN apt-get update && \
    apt-get install -y git python3-dev python3-setuptools gcc libtinfo-dev zlib1g-dev build-essential cmake libedit-dev libxml2-dev llvm

RUN git clone --recursive -b v0.7 https://github.com/apache/tvm.git /tvm && \
    cd /tvm && mkdir build && cp cmake/config.cmake build && \
    sed -i 's/set(USE_LLVM OFF)/set(USE_LLVM ON)/g' build/config.cmake && \
    cd build && cmake .. && make -j4

RUN cd /tvm/python && sed -i 's/"scipy"/"scipy==1.5.4"/g' setup.py && \
    python3 setup.py bdist_wheel && \
    python3 -m pip install /tvm/python/dist/tvm-0.7.0-cp36-cp36m-linux_x86_64.whl && \
    rm -rf /tvm

COPY --from=builder /src/dist/*.whl /tmp/model-compiler-package/

ENV INTEL_CVSDK_DIR=/opt/intel/openvino_$OPENVINO_VERSION
ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH=/usr/local/cuda-$CUDA_VERSION/targets/x86_64-linux/lib
ENV LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:$LD_LIBRARY_PATH
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

RUN python3 -m pip install /tmp/model-compiler-package/*.whl && \
    rm -r /tmp/model-compiler-package ~/.cache/pip

RUN chmod +x /script/run_compiler.sh
CMD /script/run_compiler.sh
