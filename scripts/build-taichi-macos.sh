CLANG_EXECUTABLE="$(brew list llvm@11 | grep clang++ | head -1)"

mkdir build-taichi-macos
pushd build-taichi-macos
cmake $TAICHI_REPO_DIR \
    -DCLANG_EXECUTABLE=$CLANG_EXECUTABLE \
    -DTI_WITH_C_API=ON \
    -DTI_WITH_VULKAN=ON \
    -DTI_WITH_PYTHON=OFF \
    -DTI_WITH_CC=OFF
cmake --build .
popd
