# Openvino model test results using Adlik on Intel TigerLake-U Processor(TGL-U I5-1145GRE)

## Test environment and tool description

- Image: Using Adlik serving-openvino image, the image address:
registry.cn-beijing.aliyuncs.com/adlik/serving-openvino:v0.4.0
- Model source: [Openvino downloader tool](https://docs.openvino.ai/latest/omz_tools_downloader.html),
command: ./downloader.py --name xxx --precisions yyy
- Data source: ILSVRC2012
- Openvino version: 2021.4.582
- Device: [Intel TigerLake-U](https://ark.intel.com/content/www/us/en/ark/products/codename/88759/products-formerly-tiger-lake.html)

## Note

The test carried out two schemes of Decoding + Inference and Inference only.
In the decoding process, the compressed video stream in H.264 format is decoded and
the dual-channel GPU is used.

## The latency performance of different models on Intel TigerLake-U

When testing latency performance, we use single instance. The results are as follows:

| model name      |input shape    |decoding method  |latency(ms)  |
| ----------------|:-------------:|:---------------:|:-----------:|
| ResNet-50       |[3 ,224, 224]  | --              |13.396       |
| ResNet-50 INT8  |[3 ,224, 224]  | --              |2.882        |
| YOLO v3         |[3, 416, 416]  | --              |87.15        |
| YOLO v4         |[3, 608, 608]  | --              |196.356      |
| Faster R-CNN    |[3, 600, 1024] | --              |507.062      |
| Mask R-CNN      |[3, 800, 1365] | --              |3422.098     |
| ResNet-50       |[3 ,224, 224]  | dual-channel gpu|19.743       |
| ResNet-50 INT8  |[3 ,224, 224]  | dual-channel gpu|4.212        |
| YOLO v3         |[3, 416, 416]  | dual-channel gpu|129.076      |
| YOLO v4         |[3, 608, 608]  | dual-channel gpu|288.298      |
| Faster R-CNN    |[3, 600, 1024] | dual-channel gpu|902.478      |
| Mask R-CNN      |[3, 800, 1365] | dual-channel gpu|6830.028     |

## The throughput performance of different models on Intel TigerLake-U

When testing throughput performance, we use multiple instances. The results are as follows:

| model name      | input shape   |decoding method  |throughput(pics/sec)|
| ----------------| :------------:|:---------------:|:------------------:|
| ResNet-50       | [3 ,224, 224] | --              |92.001              |
| ResNet-50 INT8  | [3 ,224, 224] | --              |714.793             |
| YOLO v3         | [3, 416, 416] | --              |12.943              |
| YOLO v4         | [3, 608, 608] | --              |5.807               |
| Faster R-CNN    | [3, 600, 1024]| --              |2.232               |
| Mask R-CNN      | [3, 800, 1365]| --              |0.298               |
| ResNet-50       | [3 ,224, 224] | dual-channel gpu|60.525              |
| ResNet-50 INT8  | [3 ,224, 224] | dual-channel gpu|515.769             |
| YOLO v3         | [3, 416, 416] | dual-channel gpu|9.181               |
| YOLO v4         | [3, 608, 608] | dual-channel gpu|3.842               |
| Faster R-CNN    | [3, 600, 1024]| dual-channel gpu|1.171               |
| Mask R-CNN      | [3, 800, 1365]| dual-channel gpu|0.146               |
