param(
    [string] $BuildType = "Debug"
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path build)) {
    New-Item -ItemType Directory build
}

Push-Location build
& cmake .. -DCMAKE_BUILD_TYPE=$BuildType
& cmake --build .
Pop-Location

& build/$BuildType/TaichiAot.exe
