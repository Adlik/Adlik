FROM ubuntu:bionic
ARG ADLIK_DIRECTORY
ARG SERVING_SCRIPT
ARG CLIENT_SCRIPT
ARG TEST_MODEL_PATH
ARG SERVING_JSON
ARG CLIENT_INFERENCE_SCRIPT
ARG IMAGE_FILENAME
ARG COMPILE_SCRIPT
ARG MODEL_NAME
ENV MODEL_NAME=${MODEL_NAME}
ENV COMPILE_SCRIPT=${COMPILE_SCRIPT}
ENV SERVING_JSON=${SERVING_JSON}
ENV CLIENT_INFERENCE_SCRIPT=${CLIENT_INFERENCE_SCRIPT}
ENV IMAGE_FILENAME=${IMAGE_FILENAME}
COPY ${ADLIK_DIRECTORY} /home/john/Adlik
COPY ${TEST_MODEL_PATH} /home/john/Adlik/model

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        make \
        patch \
        python \
        git \
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

WORKDIR /home/john

RUN bazel version

RUN pip3 install numpy

RUN cd /usr/bin &&\
    rm python &&\
    ln -s python3.6 python

RUN cd /home/john/Adlik &&\
    bazel build //adlik_serving/clients/python:build_pip_package -c opt &&\
    mkdir /tmp/pip-packages && bazel-bin/adlik_serving/clients/python/build_pip_package /tmp/pip-packages &&\
    bazel build //adlik_serving \
        --config=tensorflow-lite-cpu \
        -c opt &&\
    pip3 install --upgrade pip setuptools &&\
    pip3 install /tmp/pip-packages/adlik_serving_api-0.0.0-py2.py3-none-any.whl &&\
    cd /home/john/Adlik/model_compiler &&\
    pip3 install . &&\
    pip3 install -U pillow

COPY ${SERVING_SCRIPT} /home/john/serving_script.sh
RUN chmod +x /home/john/serving_script.sh
COPY ${CLIENT_SCRIPT} /home/john/client_script.sh
RUN chmod +x /home/john/client_script.sh
COPY ${COMPILE_SCRIPT} /home/john/compile_script.sh
RUN chmod +x /home/john/compile_script.sh

CMD python3 Adlik/benchmark/src/cmd_script.py -s /home/john/serving_script.sh -c /home/john/client_script.sh -cs /home/john/compile_script.sh

