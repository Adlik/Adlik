# Release 0.1.0

- Release Date: 2020-06-15
- Compatibility: Because r0.1.0 is the first release version of Adlik, there is no consideration on compatibility.

## Feature List

### Model Compiler

1. A new framework which is easy to expand and maintain.
2. Compilation of models trained from Keras, Tensorflow and Pytorch for better execution on CPU/GPU.

| Training framework | Model format | Target runtime | compiled format |
| ------------------ | ------------ | -------------- | --------------- |
| Keras              | h5           | Tf Serving     | SavedModel      |
|                    |              | OpenVINO       | IR              |
|                    |              | TensorRT       | Plan            |
|                    |              | TF-Lite        | tflite          |
| TensorFlow         | Ckpt/pb      | Tf Serving     | SavedModel      |
|                    |              | OpenVINO       | IR              |
|                    |              | TensorRT       | Plan            |
|                    |              | TF-Lite        | tflite          |
| PyTorch            | pth          | OpenVINO       | IR              |
|                    |              | TensorRT       | Plan            |

| Training framework | Inference engine        | hardware environment |
| ------------------ | ----------------------- | -------------------- |
| Keras              | TensorFlow Serving-1.14 | CPU/GPU              |
|                    | TensorFlow Serving-2.2  | CPU/GPU              |
|                    | OpenVINO-2019           | CPU                  |
|                    | TensorRT-6              | GPU                  |
|                    | TensorRT-7              | GPU                  |
|                    | TF Lite-2.1             | CPU(X86/ARM)         |
| TensorFlow         | TensorFlow Serving-1.14 | CPU/GPU              |
|                    | TensorFlow Serving-2.2  | CPU/GPU              |
|                    | OpenVINO-2019           | CPU                  |
|                    | TensorRT-6              | GPU                  |
|                    | TensorRT-7              | GPU                  |
|                    | TF Lite-2.1             | CPU(X86/ARM)         |
| PyTorch            | OpenVINO-2019           | CPU                  |
|                    | TensorRT-6              | GPU                  |

### Model Optimizer

1. Multi nodes multi GPUs training and pruning.
2. Configurable implementation of filter pruning to achieve smaller size of inference models.
3. Small batch dataset quantization for TF-Lite and TF-TRT.

### Inference Engine

1. Management of multi models and multi versions.
2. HTTP/GRPC interfaces for inference service.
3. Runtime scheduler that supports scheduling of multi model instances.
4. Integration of multiple DL inference runtime, including TensorFlow Serving, OpenVINO, TensorRT and TF Lite.
   | Inference engine        | hardware environment |
   | ----------------------- | -------------------: |
   | TensorFlow Serving-1.14 |              CPU/GPU |
   | TensorFlow Serving-2.2  |              CPU/GPU |
   | OpenVINO-2019           |                  CPU |
   | TensorRT-6              |                  GPU |
   | TensorRT-7              |                  GPU |
   | TF Lite-2.1             |         CPU(X86/ARM) |
5. Integration of dlib to support ML runtime.

### Benchmark Test Framework for Deep Learning Model

1. A containalized solution which should automatically execute compiling models, loading runtime and compiled
   models, starting inference service, then run inference client script, and finally generate testing results.
2. Supports all the compilers and runtime that can be integrated into Adlik.
3. Supported output: inference result, inference speed, delay of inference execution.

## Fixed issues

- [Inaccurate description in "Build in Docker".](https://github.com/Adlik/Adlik/issues/34)
- [The adlik_serving doesn't load the latest version default.](https://github.com/Adlik/Adlik/issues/59)
- [Readme documentation has no tutorial for compile model.](https://github.com/Adlik/Adlik/issues/99)
- [Bazel build does not support 2020 OpenVINO.](https://github.com/Adlik/Adlik/issues/82)
- [The docker images built by ci/docker/build.sh file can not compile openvino model.](https://github.com/Adlik/Adlik/issues/88)
- [Readme documentation has no tutorial for compile model.](https://github.com/Adlik/Adlik/issues/99)
- [If user requests an output which not in model will get null response after prediction.](https://github.com/Adlik/Adlik/issues/108)
- [Prediction will fail if information in model.pbtxt and model representation not consistent in tensorflowLite runtime.](https://github.com/Adlik/Adlik/issues/136)
