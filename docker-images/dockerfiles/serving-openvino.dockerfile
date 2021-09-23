ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base
COPY script/run_server.sh /script/run_server.sh
COPY script/opencl.sh /script/opencl.sh

ARG OPENVINO_VERSION

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates wget curl && \
    wget 'https://apt.repos.intel.com/openvino/2021/GPG-PUB-KEY-INTEL-OPENVINO-2021' -O /etc/apt/trusted.gpg.d/openvino.asc && \
    bash /script/opencl.sh && rm /script/opencl.sh && \
    apt-get autoremove --purge -y wget curl && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN echo 'deb https://apt.repos.intel.com/openvino/2021 all main' >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y "intel-openvino-ie-rt-core-$ID-$VERSION_CODENAME-$OPENVINO_VERSION" && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

# Builder.

FROM base as builder

ARG OPENVINO_VERSION

RUN apt-get update && \
    apt-get install --no-install-recommends -y wget && \
    wget 'https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg' -O /etc/apt/trusted.gpg.d/bazel.asc && \
    apt-get autoremove --purge -y wget && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN echo 'deb https://storage.googleapis.com/bazel-apt stable jdk1.8' >> /etc/apt/sources.list

RUN apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        libpython3-dev \
        libtool \
        make \
        patch \
        python3-distutils \
        python3-numpy && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

COPY . /src

WORKDIR /src

RUN env INTEL_CVSDK_DIR="/opt/intel/openvino_$OPENVINO_VERSION" \
        InferenceEngine_DIR="/opt/intel/openvino_$OPENVINO_VERSION/deployment_tools/inference_engine/share" \
        PYTHON_BIN_PATH=/usr/bin/python3 \
        bazel build --config=openvino -c opt //adlik_serving

# Runtime.

FROM base

ARG OPENVINO_VERSION

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y "intel-openvino-ie-rt-cpu-$ID-$VERSION_CODENAME-$OPENVINO_VERSION" && \
    apt-get install --no-install-recommends -y "intel-openvino-ie-rt-gpu-$ID-$VERSION_CODENAME-$OPENVINO_VERSION" && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

ENV LD_LIBRARY_PATH=/opt/intel/openvino_$OPENVINO_VERSION/deployment_tools/inference_engine/lib/intel64:/opt/intel/openvino_$OPENVINO_VERSION/deployment_tools/ngraph/lib:/opt/intel/openvino_$OPENVINO_VERSION/deployment_tools/inference_engine/external/tbb/lib

RUN chmod +x /script/run_server.sh
CMD /script/run_server.sh
