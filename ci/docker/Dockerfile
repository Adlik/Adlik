FROM ubuntu:bionic

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean

RUN apt-get install --no-install-recommends gnupg && \
    apt-key adv --fetch-keys \
        https://apt.repos.intel.com/openvino/2019/GPG-PUB-KEY-INTEL-OPENVINO-2019 \
        https://bazel.build/bazel-release.pub.gpg \
        https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64/7fa2af80.pub && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean

RUN echo 'https://apt.repos.intel.com/openvino/2019 all main\n\
https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64 /\n\
https://storage.googleapis.com/bazel-apt/ stable jdk1.8' >> /etc/apt/sources.list

RUN apt-get update &&
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        clang-format \
        clang-tidy \
        flake8 \
        iwyu \
        libclang-common-5.0-dev \
        libclang-common-6.0-dev \
        libtool \
        make \
        patch \
        python-future \
        python3-wheel && \
    apt-get clean

RUN python3 -c 'import shutil, sys, urllib.request; shutil.copyfileobj(urllib.request.urlopen(sys.argv[1]), sys.stdout.buffer)' \
        https://github.com/bazelbuild/buildtools/releases/download/0.29.0/buildifier > /usr/local/bin/buildifier && \
    chmod +x /usr/local/bin/buildifier

RUN useradd -m john

USER john

WORKDIR /home/john

RUN bazel version
