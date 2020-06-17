# TUTORIALS

## Install and build Adlik

Fllow the [Usage](adlik_serving/README.md#Usage) compile your model.

>Note:
>You need to build the Adlik serving accord to the demand scenario and environment hardware configuration. If you want
>to build TensorRT or TensorFlow GPU service, you need to make sure that your environment has the GPU and the CUDA
>driver vision >= 410.48 (click [here](https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html) for details).

## Optimize model

Fllow the [Usage](https://github.com/Adlik/model_optimizer/blob/master/README.md#usage) optimize your model.

## Compile model

Fllow the [Usage](model_compiler/README.md#usage) compile your model.

>Note:
>
>You need to pay attention that the compiled model type needs to be consistent with the build serving type.
>
>When you compile the TensorRT model, the version of the TensorRT python package must be the same as the version of
>TensorRT (`dpkg -l | grep TensorRT`)

## Deploy serving service

After you build the serving and compile the model, you can deploy the serving service.

### OpenVINO service

```sh
source /opt/intel/openvino_VERSION/bin/setupvars.sh
cd {dir_of_adlik_serving_binary}
./adlik_serving --model_base_path={model_repos_dir} --grpc_port={grpc_port} --http_port={http_port}
```

### Other service

```sh
cd {dir_of_adlik_serving_binary}
./adlik_serving --model_base_path={model_repos_dir} --grpc_port={grpc_port} --http_port={http_port}
```

## Infer model

**First**, Prepare inference client code (see [image_client.py](adlik_serving/clients/python/image_client.py)) and data
sets required for inference.

**Second**, infer the model.

```sh
python3 client.py --batch-size=1 image
```

## Test the serving engine profermance

### Automatic test the serving engine performance

**First**, use [Git](https://git-scm.com/download) clone the Adlik code or download zip.

**Second**, follow the step in [benchmark](benchmark/README.md), you can get the test result.

**Thrid**, if you want to test the performance of the new serving engine which provided by yourself, you can write a new
serving Dockerfile based on the [existing serving Dockerfile](benchmark/tests/docker_test), then following the step 2.

### Manually test serving engine performance

1. [Install and build Adlik](#Install-and-build-Adlik).

2. [Optimize models](#Optimize-model) based on demand.

3. [Compile model](#Compile-model).

4. [Deploy serving service](#Deploy-serving-service). In this way, you need to redirect log files, by:

    ```sh
    ./adlik_serving --model_base_path={model_repos_dir} --grpc_port={grpc_port} --http_port={http_port} >> log_path 2>&1
    ```

5. Run a client and do inference:

   ```sh
   cd {Adlik_root_dir}/benchmark/test/client
   python3 client.py --batch-size=128 path_image
   ```

   Save the log of client.

6. Analyze inference results

   ```sh
   cd {Adlik_root_dir}/benchmark/src
   python3 test_result.py path_client_log path_serving_log batch_size model_name runtime
   ```

   Then you can get the performance analysis results of the serving.
