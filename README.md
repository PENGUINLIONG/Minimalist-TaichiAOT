# Minimalist Taichi AOT Example

This repository presents a minimal implementation of a program with Taichi ahead-of-time (AOT) compiled modules.

## Project Structure

You might be interested in the following files.

- `app.py`: Build an AOT module and save the compilation artifacts (to the `module` directory);
- `app.cpp`: Load an AOT module, allocate resources and launch kernels;
- `CMakeLists.txt`: Find Taichi C-API library and link it to your project.

💡 **ProTip!** [Taichi C-API tests](https://github.com/taichi-dev/taichi/tree/master/c_api/tests) can be copied to `app.py` and `app.cpp` for easy debugging.

## Build for Whatever

### 1. Setup environment variables:

|Environment Variable|Description|
|-|-|
|`TAICHI_REPO_DIR`|Taichi source directory; the directory created by your `git clone` command.|
|`TAICHI_C_API_INSTALL_DIR`|CMake install directory. Something like `/path/to/taichi/_skbuild/win-amd64-3.9/cmake-install/c_api`.|
|`ANDROID_NDK_ROOT`|**(Optional)** Required for an Android build. Android NDK root directory. Something like `/Users/xxx/Library/Android/sdk/ndk/21.4.7075529`. |

### 2. Build the Taichi Runtime C-API library:

|Build Script|Description|
|-|-|
|`scripts/build-taichi-macos.sh`|Build Taichi Runtime for macOS. Vulkan backend is enabled.|
|`scripts/build-taichi-linux.sh`|Build Taichi Runtime for Linux distros. Vulkan backend is enabled.|
|`scripts/build-taichi-windows.ps1`|Build Taichi Runtime for Windows. Vulkan backend is enabled.|
|`scripts/build-taichi-android.sh`|Build Taichi Runtime for Android on a *nix host. Vulkan backend is enabled.|

Note that the Python binding is striped to save you from linking issues.

### 3. Generate AOT module with:

```bash
python app.py
```

### 4. Build this repo with:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Or call the build scripts directly:

|Build Script|Description|
|-|-|
|`scripts/run-macos.sh`|Build and run `app.cpp` on macOS.|
|`scripts/run-linux.sh`|Build and run `app.cpp` on Linux distros.|
|`scripts/run-windows.ps1`|Build and run `app.cpp` on Windows.|
|`scripts/run-android.sh`|Build and run `app.cpp` on the attached Android device via ADB.|
