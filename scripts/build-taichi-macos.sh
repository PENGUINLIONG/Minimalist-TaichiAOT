#!/bin/bash
CLANG_EXECUTABLE="$(brew list llvm@11 | grep clang++ | head -1)"

mkdir build-taichi-macos
pushd build-taichi-macos
cmake $TAICHI_REPO_DIR \
    -DCMAKE_INSTALL_PREFIX="./install" \
    -DCLANG_EXECUTABLE=$CLANG_EXECUTABLE \
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
