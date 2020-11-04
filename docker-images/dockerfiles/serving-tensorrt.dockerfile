ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base

ARG TENSORRT_VERSION
ARG CUDA_VERSION
ENV TENSORRT_VERSION=${TENSORRT_VERSION}
ENV CUDA_VERSION=${CUDA_VERSION}

COPY script/cuda.sh /script/cuda.sh

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y wget ca-certificates && \
    wget "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" -O /etc/apt/trusted.gpg.d/cuda.asc && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /" >> /etc/apt/sources.list

RUN chmod +x /script/cuda.sh
RUN ./script/cuda.sh

# Builder.

FROM base as builder

RUN . /etc/os-release && \
    apt-get update && \
    wget 'https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg' -O /etc/apt/trusted.gpg.d/bazel.asc && \
    apt-get autoremove --purge -y wget && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN echo 'deb https://storage.googleapis.com/bazel-apt stable jdk1.8' >> /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
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
         --incompatible_use_specific_tool_files=false

# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

CMD ["adlik-serving", "--grpc_port=8500", "--http_port=8501", "--model_base_path=/srv/adlik-serving"]
