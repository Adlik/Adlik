ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base

ARG TENSORRT_VERSION
ARG CUDA_VERSION
ENV TENSORRT_VERSION=${TENSORRT_VERSION}
ENV CUDA_VERSION=${CUDA_VERSION}

COPY script/cuda.sh /script/cuda.sh
COPY script/run_server.sh /script/run_server.sh

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

RUN chmod +x /script/cuda.sh
RUN ./script/cuda.sh

# Builder.

FROM base as builder

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y wget && \
    wget 'https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg' -O /etc/apt/trusted.gpg.d/bazel.asc && \
    apt-get autoremove --purge -y wget && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN echo 'deb https://storage.googleapis.com/bazel-apt stable jdk1.8' >> /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        git \
        automake \
        libpython2.7-stdlib \
        bazel \
        libpython3-dev \
        libtool \
        make \
        patch \
        python-minimal \
        python3-distutils \
        python3-numpy && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

COPY . /src

WORKDIR /src

RUN env PYTHON_BIN_PATH=/usr/bin/python3 \
        TF_CUDA_VERSION=${CUDA_VERSION} \
        bazel build //adlik_serving \
         --config=tensorrt \
         -c opt \
         --action_env=LIBRARY_PATH=/usr/local/cuda-${CUDA_VERSION}/lib64/stubs \
         --incompatible_use_specific_tool_files=false \
         --copt="-Ithird_party/cuda/include"

# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:$LD_LIBRARY_PATH
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

RUN chmod +x /script/run_server.sh
CMD /script/run_server.sh
