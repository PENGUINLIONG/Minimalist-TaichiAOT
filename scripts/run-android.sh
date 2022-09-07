mkdir build-android-aarch64
pushd build-android-aarch64
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" \
    -DANDROID_PLATFORM=android-26 \
    -DANDROID_ABI="arm64-v8a"
popd

adb shell mkdir /data/local/tmp/taichi-aot/
adb push ./build-android-aarch64/TaichiAot /data/local/tmp/taichi-aot/
adb push ./build-android-aarch64/libtaichi_c_api.so /data/local/tmp/taichi-aot/
adb push ./module /data/local/tmp/taichi-aot/
adb push ./scripts/__android_main.sh /data/local/tmp/taichi-aot/
adb shell sh /data/local/tmp/taichi-aot/__android_main.sh