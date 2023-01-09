#!/bin/bash

if (-not (Test-Path "$env:TAICHI_REPO_DIR")) {
    Write-Host "Please set TAICHI_REPO_DIR env variable"
    exit
} else {
    Write-Host "TAICHI_REPO_DIR is set to ${TAICHI_REPO_DIR}"
}

if (-not (Test-Path "$env:ANDROID_NDK_ROOT")) {
    Write-Host "Please set ANDROID_NDK_ROOT env variable"
    exit
} else {
    Write-Host "ANDROID_NDK_ROOT is set to ${ANDROID_NDK_ROOT}"
}

Remove-Item -Recurse -Force "build-taichi-android-aarch64"
New-Item -ItemType Directory "build-taichi-android-aarch64"
Push-Location "build-taichi-android-aarch64"
cmake $env:TAICHI_REPO_DIR `
    -DCMAKE_BUILD_TYPE=Release `
    -DCMAKE_INSTALL_PREFIX="./install" `
    -DCMAKE_TOOLCHAIN_FILE="$env:ANDROID_NDK_ROOT/build/cmake/android.toolchain.cmake" `
    -DANDROID_ABI="arm64-v8a" `
    -DANDROID_PLATFORM=android-26 `
    -G "Ninja" `
    -DTI_WITH_CC=OFF `
    -DTI_WITH_CUDA=OFF `
    -DTI_WITH_CUDA_TOOLKIT=OFF `
    -DTI_WITH_C_API=ON `
    -DTI_WITH_DX11=OFF `
    -DTI_WITH_LLVM=OFF `
    -DTI_WITH_METAL=OFF `
    -DTI_WITH_OPENGL=OFF `
    -DTI_WITH_PYTHON=OFF `
    -DTI_WITH_VULKAN=ON

cmake --build . -t taichi_c_api
cmake --build . -t install
Pop-Location
