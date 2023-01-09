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

& build-windows/$BuildType/TaichiAot.exe
