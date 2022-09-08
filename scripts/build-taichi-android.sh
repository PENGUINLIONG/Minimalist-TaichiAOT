CLANG_EXECUTABLE=$(find $ANDROID_NDK_ROOT -name "clang++")

mkdir build-taichi-android-aarch64
pushd build-taichi-android-aarch64
cmake $TAICHI_REPO_DIR \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="./build-taichi-android-aarch64/install" \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DCLANG_EXECUTABLE=$CLANG_EXECUTABLE \
    -DANDROID_ABI="arm64-v8a" \
    -DANDROID_PLATFORM=android-26 \
    -G "Ninja" \
    -DTI_WITH_CC=OFF \
    -DTI_WITH_CUDA=OFF \
    -DTI_WITH_CUDA_TOOLKIT=OFF \
    -DTI_WITH_C_API=ON \
    -DTI_WITH_DX11=OFF \
    -DTI_WITH_LLVM=OFF \
    -DTI_WITH_METAL=OFF \
    -DTI_WITH_OPENGL=OFF \
    -DTI_WITH_PYTHON=OFF \
    -DTI_WITH_VULKAN=ON

cmake --build . -t taichi_c_api
cmake --build . -t install
popd
