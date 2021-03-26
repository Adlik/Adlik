# PaddlePaddle model test result using Adlik

In the test, the batch size of model is 1. The CPU and GPU parameters are as follows:

|     |                   type                    | number |
| --- | :---------------------------------------: | :----: |
| CPU | Intel(R) Xeon(R) CPU E5-2680 v4 @ 2.40GHz |   6    |
| GPU |           Tesla T4 15109MiB               |   1    |

## Different models test result in CPU

When using Adlik for model inference, the PaddlePaddle model is deployed in OpenVINO type.

| model     | using PaddlePaddle (ms)     | using Adlik(ms) |
| --------- | :-------------------------: | :-------: |
| PPocr_det |                             |   105.82  |
| PPocr_rec |                             |   18.91   |
|PPocr_cls  |                             |   96.02   |
| PPresnet50 |                            |   16.36   |

## Different models test result in GPU

When using Adlik for model inference, the PaddlePaddle model is deployed in TensorRT type.

| model     | using PaddlePaddle (ms)     | using Adlik(ms) |
| --------- | :-------------------------: | :-------: |
| PPocr_det |                             |   33.51  |
| PPocr_rec |                             |   13.72  |
| PPocr_cls |                             |   1.56   |
| PPresnet50 |                            |   4.93   |
| PPyolo_mobile_net |                     |   4.20   |
