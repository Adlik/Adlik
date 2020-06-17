FROM ubuntu:bionic
ARG ADLIK_DIRECTORY
ARG OPENVINO_VERSION
ARG SERVING_SCRIPT
ARG CLIENT_SCRIPT
ARG TEST_MODEL_PATH
ARG SERVING_JSON
ARG CLIENT_INFERENCE_SCRIPT
ARG IMAGE_FILENAME
ARG COMPILE_SCRIPT
ENV COMPILE_SCRIPT=${COMPILE_SCRIPT}
ENV SERVING_JSON=${SERVING_JSON}
ENV OPENVINO_VERSION=${OPENVINO_VERSION}
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
        https://apt.repos.intel.com/openvino/2019/GPG-PUB-KEY-INTEL-OPENVINO-2019 \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://apt.repos.intel.com/openvino/2019 all main\n\
deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        patch \
        git \
        make \
        intel-openvino-runtime-ubuntu18-$OPENVINO_VERSION \
        intel-openvino-dev-ubuntu18-$OPENVINO_VERSION \
        libtbb2 \
        libtool \	
        python3-setuptools \
        python3-wheel \
        python3.7-dev \
        python3-six \
        python3-pip && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

WORKDIR /home/john

RUN bazel version

RUN cd /home/john/Adlik &&\
    bazel build //adlik_serving/clients/python:build_pip_package -c opt &&\
    mkdir /tmp/pip-packages && bazel-bin/adlik_serving/clients/python/build_pip_package /tmp/pip-packages &&\
    export INTEL_CVSDK_DIR=/opt/intel/openvino_${OPENVINO_VERSION}/ &&\
    export InferenceEngine_DIR=$INTEL_CVSDK_DIR/deployment_tools/inference_engine/share &&\
    bazel build //adlik_serving \
        --config=openvino \
        -c opt &&\
    pip3 install --upgrade pip &&\
    pip3 install /tmp/pip-packages/adlik_serving_api-0.0.0-py2.py3-none-any.whl &&\
    cd /home/john/Adlik/model_compiler &&\
    pip3 install . &&\
    pip3 install -U tensorflow==1.14 defusedxml==0.5.0 networkx==2.3.0 pillow

COPY ${SERVING_SCRIPT} /home/john/serving_script.sh
RUN chmod +x /home/john/serving_script.sh
COPY ${CLIENT_SCRIPT} /home/john/client_script.sh
RUN chmod +x /home/john/client_script.sh
COPY ${COMPILE_SCRIPT} /home/john/compile_script.sh
RUN chmod +x /home/john/compile_script.sh

CMD python3 Adlik/benchmark/src/cmd_script.py -s /home/john/serving_script.sh -c /home/john/serving_script.sh -cs /home/john/compile_script.sh
