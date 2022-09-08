mkdir build-macos
pushd build-macos
cmake ..
cmake --build .
popd

TI_LIB_DIR=./build-taichi-macos ./build-macos/TaichiAot
