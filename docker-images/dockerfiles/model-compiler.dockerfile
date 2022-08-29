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

ARG TENSORRT_VERSION
ARG CUDA_VERSION
ENV TENSORRT_VERSION=${TENSORRT_VERSION}
ENV CUDA_VERSION=${CUDA_VERSION}

RUN . /etc/os-release && \
	apt-get update && \
	apt-get install --no-install-recommends -y gnupg curl software-properties-common && \
	curl "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/3bf863cc.pub" | apt-key add - && \
	curl "https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" | apt-key add - && \
	apt-add-repository "deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /" && \
	apt-add-repository -u "deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /" && \
	apt-get autoremove --purge -y curl gnupg software-properties-common && \
	apt-get clean && \
	find /var/lib/apt/lists -delete

RUN chmod +x /script/tensorrt.sh
RUN /script/tensorrt.sh

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        libgl1 libgtk-3-0 && \
    apt-get clean && \
    find /var/lib/apt/lists -delete &&\
    ln -s /usr/local/cuda-"$CUDA_VERSION" /usr/local/cuda

RUN apt-get update && apt-get install -y git gcc python3-dev libxml2-dev libxslt1-dev zlib1g-dev && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

COPY --from=builder /src/dist/*.whl /tmp/model-compiler-package/

RUN env PIP_FIND_LINKS='https://release.oneflow.info/' python3 -m pip install /tmp/model-compiler-package/*.whl && \
    rm -r /tmp/model-compiler-package ~/.cache/pip

ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH=/usr/local/cuda-$CUDA_VERSION/targets/x86_64-linux/lib:$LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:$LD_LIBRARY_PATH
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
ENV CPLUS_INCLUDE_PATH=/usr/include/python3.8m:$CPLUS_INCLUDE_PATH

RUN chmod +x /script/run_compiler.sh
CMD /script/run_compiler.sh
