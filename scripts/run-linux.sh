#!/bin/bash
set -e

rm -rf build-linux
mkdir build-linux
pushd build-linux
TAICHI_C_API_INSTALL_DIR="${PWD}/../build-taichi-linux/install/c_api" cmake ..
cmake --build .
popd

if [[ ! -f "./module/metadata.tcb" ]]; then
    echo "Did you generated AOT module?"
    exit -1
fi

./build-linux/TaichiAot
