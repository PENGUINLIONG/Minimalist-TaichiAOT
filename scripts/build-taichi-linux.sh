#!/bin/bash
set -e

if [[ -z "${TAICHI_REPO_DIR}" ]]; then
    echo "Please set TAICHI_REPO_DIR env variable"
    exit
else
    echo "TAICHI_REPO_DIR is set to ${TAICHI_REPO_DIR}"
fi

rm -rf build-taichi-linux
mkdir build-taichi-linux
pushd build-taichi-linux
cmake $TAICHI_REPO_DIR \
    -DCMAKE_INSTALL_PREFIX="./install" \
    -G "Ninja" \
    -DTI_WITH_C_API=ON \
    -DTI_WITH_VULKAN=ON \
    -DTI_WITH_CPU=OFF \
    -DTI_WITH_LLVM=OFF \
    -DTI_WITH_CUDA=OFF \
    -DTI_WITH_PYTHON=OFF \
    -DTI_WITH_CC=OFF
cmake --build . -t taichi_c_api
cmake --build . -t install
popd
