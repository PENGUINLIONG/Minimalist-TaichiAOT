param(
    [string] $BuildType = "Debug"
)

$env:TAICHI_C_API_INSTALL_DIR = "build-taichi-windows\install\c_api";

$ErrorActionPreference = "Stop"
if (-not (Test-Path "build-windows")) {
    New-Item -ItemType Directory "build-windows"
}

Push-Location "build-windows"
& cmake .. -DCMAKE_BUILD_TYPE="$BuildType"
& cmake --build .
Pop-Location

Copy-Item build-taichi-windows/$BuildType/taichi_c_api.dll build-windows/$BuildType/taichi_c_api.dll -Force
& build-windows/$BuildType/TaichiAot.exe
