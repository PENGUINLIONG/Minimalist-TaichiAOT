#!/bin/bash
set -e

rm -rf build-macos
mkdir build-macos
pushd build-macos
cmake ..
cmake --build .
popd

if [[ ! -f "./module/metadata.tcb" ]]; then
    echo "Did you run python app.py --arch=vulkan?"
    exit -1
fi

TI_LIB_DIR=./build-macos ./build-macos/TaichiAot
