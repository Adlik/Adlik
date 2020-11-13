ARG UBUNTU_VERSION

# Base.

FROM "ubuntu:$UBUNTU_VERSION" as base

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
        libpython2.7-stdlib \
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
        bazel build --config=tensorflow-lite-cpu -c opt //adlik_serving

# Runtime.

FROM base

COPY --from=builder /src/bazel-bin/adlik_serving/adlik_serving /usr/local/bin/adlik-serving

CMD ["adlik-serving", "--grpc_port=8500", "--http_port=8501", "--model_base_path=/srv/adlik-serving"]