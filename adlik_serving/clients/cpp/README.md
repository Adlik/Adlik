# About the build of Adlik cpp client

1. Build the cpp_client with bazel. The build process in this way relies on Adlik.

    First, install [bazel](https://docs.bazel.build/install.html) and some packages needed for building, refer to Adlik [Build](https://github.com/Adlik/Adlik#build).

    Then, build cpp_client.

    ```sh
    cd Adlik
    bazel build //adlik_serving/clients/cpp:cpp_client -c opt
    ```

2. Build the cpp_client with cmake. The method can be built separately with minimal dependencies.

    First, install [grpc](https://grpc.io/docs/languages/cpp/quickstart/#install-grpc). Among them, the protobuf
    version is required to be 3.9.2, and the cmake version>=3.13.

    Then, Compile and generate Adlik C++ grpc client source files.

    ```sh
    cd Adlik/adlik_serving/clients
    protoc --proto_path=cpp --cpp_out=cpp cpp/adlik_serving/apis/*.proto cpp/tensorflow/core/framework/*.proto cpp/adlik_serving/framework/domain/*.proto
    protoc --proto_path=cpp --grpc_out=cpp --plugin protoc-gen-grpc=`which grpc_cpp_plugin` cpp/adlik_serving/apis/*.
    proto cpp/tensorflow/core/framework/*.proto cpp/adlik_serving/framework/domain/*.proto
    ```

    Last, build cpp_client.

    ```sh
    cd cpp
    mkdir build && cd build
    cmake ..
    make
    ```
