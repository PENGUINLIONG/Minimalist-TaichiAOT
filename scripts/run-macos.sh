#!/bin/bash
set -e

rm -rf build-macos
mkdir build-macos
pushd build-macos
TAICHI_C_API_INSTALL_DIR="${PWD}/../build-taichi-macos/install/c_api" cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"
cmake --build .
popd

if [[ ! -f "./module/metadata.tcb" ]]; then
    echo "Did you generated AOT module?"
    exit -1
fi

TI_LIB_DIR=./build-macos ./build-macos/TaichiAot
