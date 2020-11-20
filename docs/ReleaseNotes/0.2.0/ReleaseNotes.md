# Release 0.2.0

- Release Date: 2020-11-20
- Compatibility: The functional interfaces of Adlik r0.2 are compatible with r0.1.

## Feature List

### New Model Compiler

1. Support DAG generation for end-to-end compilation of models with different representation.
2. Source representation: H5, Ckpt, Pb, Pth, Onnx and SavedModel.
3. Target representation: SavedModel, OpenVINO IR, TensorRT Plan and Tflite.
4. Support model quantization for TfLite and TensorRT.
5. Int8 quantization for TfLite.
6. Int8 and fp16 quantization for TensorRT.

### Inference Engine

1. Support hybrid scheduling of ML and DL inference jobs.
2. Support image based deployment of Adlik compiler and inference engine in cloud native environment.
3. Deployment and functions has been tested in docker (V19.03.12) and Kubernetes (V1.13).
4. Support Adlik running in Raspberry Pi and JetsonNano.
5. Support the newest version of OpenVINO (2021.1.110) and TensorFlow (2.3.1).

### Benchmark Test

1. Support benchmark test for models including ResNet-50, Inception V3, Yolo V3 and Bert with runtimes supported by Adlik.

## Fixed issues

- [Can Not Convert Yolo.h5 To Openvino Runtime.](https://github.com/Adlik/Adlik/issues/299)
- [gRPC:Received message larger than max.](https://github.com/Adlik/Adlik/issues/292)
- [Return Message Is Wrong When cudaMalloc() Failed In initializeOutputBindings() Method.](https://github.com/Adlik/Adlik/issues/287)
- [Can Not Do Predict With Following Transferred YoloV3 Model.](https://github.com/Adlik/Adlik/issues/286)
- [`adlik_serving --help` should exit successfully.](https://github.com/Adlik/Adlik/issues/269)
- [benchmark cant auto infer by tensorflow gpu image.](https://github.com/Adlik/Adlik/issues/217)
- [Prediction will fail if information in model.pbtxt and model representation not consistent in tensorflowLite runtime.](https://github.com/Adlik/Adlik/issues/136)
