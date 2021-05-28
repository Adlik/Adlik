#!/usr/bin/env bash

git clone --recursive -b v0.7 https://github.com/apache/tvm.git tvm

apt-get update && apt-get install -y python3-setuptools zlib1g-dev cmake libedit-dev libxml2-dev llvm
cd tvm && mkdir build && cp cmake/config.cmake build
sed -i 's/set(USE_LLVM OFF)/set(USE_LLVM ON)/g' build/config.cmake
cd build && cmake .. && make -j4
cd ../python && sed -i 's/"scipy"/"scipy==1.5.4"/g' setup.py
python3 setup.py bdist_wheel
python3 -m pip install dist/tvm-0.7.0-cp36-cp36m-linux_x86_64.whl
cd ../.. && rm -rf tvm
