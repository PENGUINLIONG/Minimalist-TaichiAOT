if (-not (Test-Path $env:TAICHI_REPO_DIR)) {
    Write-Host "Please set TAICHI_REPO_DIR env variable"
    exit
} else {
    Write-Host "TAICHI_REPO_DIR is set to $env:TAICHI_REPO_DIR"
}

if (-not (Test-Path "build-taichi-windows")) {
    New-Item "build-taichi-windows" -ItemType Directory
}

$CLANG_EXECUTABLE = "$env:TAICHI_REPO_DIR/tmp\taichi-clang\bin\clang++.exe"

Push-Location "build-taichi-windows"
& cmake "$env:TAICHI_REPO_DIR" `
    -DCMAKE_INSTALL_PREFIX="./install" `
    -DCLANG_EXECUTABLE="$CLANG_EXECUTABLE" `
    -DTI_WITH_C_API=ON `
    -DTI_WITH_VULKAN=ON `
    -DTI_WITH_LLVM=OFF `
    -DTI_WITH_CUDA=OFF `
    -DTI_WITH_PYTHON=OFF `
    -DTI_WITH_CC=OFF
& cmake --build . -t taichi_c_api
& cmake --build . -t install
Pop-Location
