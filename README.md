# Adlik

[![Build Status](https://dev.azure.com/ZTE-Corporation/GitHub/_apis/build/status/ZTE.Adlik?branchName=master)](https://dev.azure.com/ZTE-Corporation/GitHub/_build/latest?definitionId=1&branchName=master)

***Adlik*** is an end-to-end optimizing framework for deep learning models. The goal of Adlik is to accelerate deep
learning inference process both on cloud and embedded environment.

***Adlik*** consists of two sub projects: Model compiler and Serving platform.

***Model compiler*** supports several optimizing technologies like pruning, quantization and structural compression,
which can be easily used for models developed with TensorFlow, Keras, PyTorch, etc.

***Serving platform*** provides deep learning models with optimized runtime based on the deployment environment. Put
simply, based on a deep learning model, the users of Adlik can optimize it with model compiler and then deploy it to a
certain platform with Adlik serving platform.

![Adlik schematic diagram](resources/adlik.png)

With Adlik framework, different deep learning models can be deployed to different platforms with high performance in a
much flexible and easy way.

## Adlik: Model Compiler

![Model Compiler schematic diagram](resources/model-compiler.png)

1. Support optimization for models from different kinds of deep learning architecture, eg. TensorFlow/Caffe/PyTorch.
2. Support compiling models as different formats, OpenVINO IR/ONNX/TensorRT for different runtime, eg. CPU/GPU/FPGA.
3. Simplified interfaces for the workflow.

## Adlik: Serving Engine

![Serving Engine schematic diagram](resources/serving-engine.png)

1. Model uploading & upgrading, model inference & monitoring.
2. Unified inference interfaces for different models.
3. Management and scheduling for a solution with multiple models in various runtime.
4. Automatic selection of inference runtime.
5. Ability to add customized runtime.

## How to build

1. Install Git and [Bazel](https://docs.bazel.build/install.html).
2. Run the following command:

   ```bash
   git clone https://github.com/ZTE/Adlik.git
   cd Adlik
   bazel build //adlik_serving
   ```
