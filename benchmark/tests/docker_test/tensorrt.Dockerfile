FROM ubuntu:bionic
ARG ADLIK_DIRECTORY
ARG SERVING_SCRIPT
ARG CLIENT_SCRIPT
ARG TEST_MODEL_PATH
ARG SERVING_JSON
ARG CLIENT_INFERENCE_SCRIPT
ARG IMAGE_FILENAME
ARG TENSORRT_VERSION
ARG TENSORRT_TAR
ARG COMPILE_SCRIPT
ARG MODEL_NAME
ENV MODEL_NAME=${MODEL_NAME}
ENV COMPILE_SCRIPT=${COMPILE_SCRIPT}
ENV CLIENT_INFERENCE_SCRIPT=${CLIENT_INFERENCE_SCRIPT}
ENV IMAGE_FILENAME=${IMAGE_FILENAME}
ENV TENSORRT_VERSION=${TENSORRT_VERSION}
ENV SERVING_JSON=${SERVING_JSON}
COPY ${ADLIK_DIRECTORY} /home/john/Adlik
COPY ${TEST_MODEL_PATH} /home/john/Adlik/model

RUN mkdir /home/john/tensorrt
COPY ${TENSORRT_TAR} /home/john/tensorrt

RUN apt-get update && \
    apt-get install --no-install-recommends -y ca-certificates && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y gnupg && \
    apt-key adv --fetch-keys \
        "https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64/7fa2af80.pub" \
        https://storage.googleapis.com/bazel-apt/doc/apt-key.pub.gpg && \
    apt-get autoremove --purge -y gnupg && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN . /etc/os-release && \
    echo "deb https://developer.download.nvidia.com/compute/cuda/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://developer.download.nvidia.com/compute/machine-learning/repos/$ID$(echo $VERSION_ID | tr -d .)/x86_64 /\n\
deb https://storage.googleapis.com/bazel-apt stable jdk1.8" >> /etc/apt/sources.list

RUN . /etc/os-release && \
    apt-get update && \
    apt-get install --no-install-recommends -y \
        automake \
        bazel \
        make \
        patch \
        cuda-cublas-dev-10-0 \
        cuda-cufft-dev-10-0 \
        cuda-cupti-10-0 \
        cuda-curand-dev-10-0 \
        cuda-cusolver-dev-10-0 \
        cuda-cusparse-dev-10-0 \
        cuda-nvml-dev-10-0 \
        cuda-nvrtc-10-0 \
        git \
        libtbb2 \
        'libcudnn7=*+cuda10.0' \
        'libcudnn7-dev=*+cuda10.0' \
        'libnvinfer7=*+cuda10.0' \
        'libnvinfer-dev=*+cuda10.0' \
        'libnvonnxparsers7=*+cuda10.0' \
        'libnvonnxparsers-dev=*+cuda10.0' \
        libtool \
        python \
        python3-setuptools \
        python3-wheel \
        python3.7-dev \
        python3-six \
        python3-pip && \
    apt-get clean && \
    find /var/lib/apt/lists -delete

RUN apt-mark hold libcudnn7 libcudnn7-dev libnvinfer7 libnvinfer-dev libnvonnxparsers7 libnvonnxparsers-dev

WORKDIR /home/john

RUN bazel version

RUN pip3 install numpy

RUN cd /usr/bin &&\
    rm python &&\
    ln -s python3.6 python

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility

RUN cd /home/john/tensorrt &&\
    tar xzvf ${TENSORRT_TAR} &&\
    cd TensorRT-${TENSORRT_VERSION}/python &&\
    pip3 install tensorrt-${TENSORRT_VERSION}-cp36-none-linux_x86_64.whl &&\
    cd /home/john/tensorrt/TensorRT-${TENSORRT_VERSION}/uff &&\
    pip3 install uff-*.whl &&\
    cd /home/john/tensorrt/TensorRT-${TENSORRT_VERSION}/graphsurgeon &&\
    pip3 install graphsurgeon-*.whl

ENV LD_LIBRARY_PATH=/home/john/tensorrt/TensorRT-${TENSORRT_VERSION}/lib

RUN cd /home/john/Adlik &&\
    bazel build //adlik_serving/clients/python:build_pip_package -c opt &&\
    mkdir /tmp/pip-packages && bazel-bin/adlik_serving/clients/python/build_pip_package /tmp/pip-packages &&\
    env TF_CUDA_VERSION=10.2 \
     bazel build //adlik_serving \
         --config=tensorrt \
         -c opt \
         --action_env=LIBRARY_PATH=/usr/local/cuda-10.0/lib64/stubs \
         --incompatible_use_specific_tool_files=false &&\
    pip3 install --upgrade pip &&\
    pip3 install /tmp/pip-packages/adlik_serving_api-0.0.0-py2.py3-none-any.whl &&\
    cd /home/john/Adlik/model_compiler_2 &&\
    pip3 install . &&\
    pip3 install -U tensorflow==1.14 pillow

COPY ${SERVING_SCRIPT} /home/john/serving_script.sh
RUN chmod +x /home/john/serving_script.sh
COPY ${CLIENT_SCRIPT} /home/john/client_script.sh
RUN chmod +x /home/john/client_script.sh
COPY ${COMPILE_SCRIPT} /home/john/compile_script.sh
RUN chmod +x /home/john/compile_script.sh

CMD python3 Adlik/benchmark/src/cmd_script.py -s /home/john/serving_script.sh -c /home/john/client_script.sh -cs /home/john/compile_script.sh
