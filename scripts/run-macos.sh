#!/bin/bash
mkdir build-macos
pushd build-macos
cmake ..
cmake --build .
popd

TI_LIB_DIR=./build-macos ./build-macos/TaichiAot
