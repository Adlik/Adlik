# Release 0.3.0

Release Date: 2021-06-21
Compatibility: The functional interfaces of Adlik r0.3 are compatible with r0.2 and r0.1.

## Feature List

### Compiler

 1. Integrate deep learning frameworks including PaddlePaddle, Caffe and MXNet
 2. Support compiling into TVM
 3. Support FP16 quantization for OpenVINO
 4. Support TVM auto scheduling

### Optimizer

 1. [Specific optimization for YOLO V4](https://github.com/Adlik/object_detection/blob/main/README.md)
 2. [Pruning, distillation and quantization for ResNet-50](https://github.com/Adlik/model_optimizer/blob/master/README.md)

### Inference Engine

 1. Support runtime of TVM and TF-TRT
 2. Docker images for cloud native environments support newest version of inference components including:

- OpenVINO (2021.1.110)
- TensorFlow (2.4.0)
- TensorRT (7.2.1.6)
- TFLite (2.4.0)
- TVM (0.7)

### Benchmark Test

 1. Support paddle models, such as Paddle OCR，PP-YOLO，PPresnet-50

## Fixed issues

- [Resnet50 is compiled to the saved model, the report error op is not in the graph.](https://github.com/Adlik/Adlik/issues/299)
- [Error method to resolve env list in tflite.](https://github.com/Adlik/Adlik/issues/389)
- [The client does not support grpc domain.](https://github.com/Adlik/Adlik/issues/402)
- [Failed to compile custom saved model to tflite.](https://github.com/Adlik/Adlik/issues/409)
- [Failed to compile onnx model to OpenVINO model.](https://github.com/Adlik/Adlik/issues/430)
- [The client does not support grpc domain.](https://github.com/Adlik/Adlik/issues/402)
- [build fails on AArch64, Fedora 33.](https://github.com/Adlik/Adlik/issues/422)
