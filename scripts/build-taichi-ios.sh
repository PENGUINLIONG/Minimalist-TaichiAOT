#!/bin/bash
set -e
CLANG_EXECUTABLE="$(brew list llvm@11 | grep clang++ | head -1)"

if [[ -z "${TAICHI_REPO_DIR}" ]]; then
    echo "Please set TAICHI_REPO_DIR env variable"
    exit
else
    echo "TAICHI_REPO_DIR is set to ${TAICHI_REPO_DIR}"
fi

if [[ -z "tmp/ios.toolchain.cmake" ]]; then
    if [[ -z "tmp" ]]; then
        mkdir tmp
    fi
    wget https://raw.githubusercontent.com/leetal/ios-cmake/master/ios.toolchain.cmake -o tmp/ios.toolchain.cmake
fi

#rm -rf build-taichi-ios-arm64
#mkdir build-taichi-ios-arm64
pushd build-taichi-ios-arm64
# See https://stackoverflow.com/questions/12630970/compiling-for-ios-with-cmake
cmake $TAICHI_REPO_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE="tmp/ios.toolchain.cmake" \
    -DDEPLOYMENT_TARGET=13.0 \
    -DPLATFORM=OS64 \
    -DCMAKE_INSTALL_PREFIX="./install" \
    -DCLANG_EXECUTABLE=$CLANG_EXECUTABLE \
    -G "Xcode" \
    -DTI_WITH_C_API=ON \
    -DTI_C_API_BUILD_STATIC=ON \
    -DTI_WITH_VULKAN=ON \
    -DTI_WITH_METAL2=ON \
    -DTI_WITH_METAL=OFF \
    -DTI_WITH_OPENGL=OFF \
    -DTI_WITH_LLVM=OFF \
    -DTI_WITH_CUDA=OFF \
    -DTI_WITH_PYTHON=OFF \
    -DTI_WITH_GGUI=OFF \
    -DTI_WITH_CC=OFF
cmake --build . -t taichi_c_api
cmake --build . -t install
popd
