ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base
COPY script/run_server.sh /script/run_server.sh

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
        build-essential \
        cmake \
        git \
        libpython3-dev \
        libtool \
        make \
        patch \
        python3-distutils \
        python3-numpy && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN cd /usr && git clone --recursive -b v0.7 https://github.com/apache/tvm.git tvm && \
    cd tvm && mkdir build && cp cmake/config.cmake build && \
    cd build && cmake .. && make runtime && \
    mv libtvm_runtime.so /usr/local/lib && \
    cd .. && mv include/tvm  3rdparty/dlpack/include/dlpack 3rdparty/dmlc-core/include/dmlc /usr/local/include && \
    cd .. && rm -rf tvm

COPY . /src

WORKDIR /src

RUN env PYTHON_BIN_PATH=/usr/bin/python3 \
        bazel build --config=tvm-cpu -c opt //adlik_serving

# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

RUN chmod +x /script/run_server.sh
CMD /script/run_server.sh
