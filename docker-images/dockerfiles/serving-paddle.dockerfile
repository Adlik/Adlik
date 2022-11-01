ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base
COPY script/run_server.sh /script/run_server.sh
RUN apt-get update && \
    apt-get install --no-install-recommends -y libgomp1 && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

# Builder.

FROM base as builder

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

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
        cmake \
        git \
        libpython3-dev \
        libtool \
        make \
        patch \
        patchelf \
        python3-minimal \
        python3-distutils \
        python3-pip \
        python3-numpy \
        unrar \
        wget && \
    apt-get clean && \
    find /var/lib/apt/lists -delete && \
    python3 -m pip install protobuf==3.19.4 wheel

COPY . /src

WORKDIR /src

RUN env PYTHON_BIN_PATH=/usr/bin/python3 \
        bazel build --config=paddle -c opt //adlik_serving \
        --action_env=http_proxy --action_env=https_proxy

RUN mkdir -p /tmp/lib && \
    cp -H /src/bazel-bin/adlik_serving/adlik_serving.runfiles/adlik/_solib_k8/*/* /tmp/lib


# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving
COPY --from=builder /tmp/lib/libmklml_intel.so /usr/local/lib/libmklml_intel.so
COPY --from=builder /tmp/lib/libiomp5.so /usr/local/lib/libiomp5.so
COPY --from=builder /tmp/lib/libdnnl.so.2 /usr/local/lib/libdnnl.so.2

RUN chmod +x /script/run_server.sh && ldconfig
CMD /script/run_server.sh
