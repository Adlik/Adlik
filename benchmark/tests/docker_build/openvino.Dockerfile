FROM ubuntu:bionic
RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://apt.repos.intel.com/openvino/2022 $VERSION_CODENAME main\n\
deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        patch \
        git \
        make \
        libopenvino-dev-2022.1.0 \
        libtbb2 \
        libtool \
        python3-setuptools \
        python3-wheel \
        python3.6-dev \
        python3-six \
        python3-pip \
        protobuf-compiler && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN useradd -m john

USER john

WORKDIR /home/john

RUN bazel version
