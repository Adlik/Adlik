ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base
COPY script/run_server.sh /script/run_server.sh

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y wget ca-certificates && \
    wget "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" -O /etc/apt/trusted.gpg.d/cuda.asc && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /" >> /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        cuda-cufft-10-2 \
        cuda-cufft-dev-10-2 \
        cuda-cupti-dev-10-2 \
        cuda-curand-10-2 \
        cuda-curand-dev-10-2 \
        cuda-cusolver-10-2 \
        cuda-cusolver-dev-10-2 \
        cuda-cusparse-dev-10-2 \
        cuda-cusparse-10-2 \
        cuda-nvml-dev-10-2 \
        cuda-nvrtc-10-2 \
        libcublas-dev=10.2.* \
        'libcudnn7=*+cuda10.2' \
        'libcudnn7-dev=*+cuda10.2' && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcudnn7 libcudnn7-dev

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
        TF_CUDA_VERSION=10.2 \
        bazel build //adlik_serving \
         --config=tensorflow-gpu \
         -c opt \
         --incompatible_use_specific_tool_files=false

# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

ENV PATH /usr/local/nvidia/bin:/usr/local/cuda/bin:${PATH}
ENV LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:$LD_LIBRARY_PATH
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

RUN chmod +x /script/run_server.sh
CMD /script/run_server.sh
