# Minimalist Taichi AOT Example

This repository presents a minimal implementation of a program with Taichi ahead-of-time (AOT) compiled modules.

# Project Structure

You might be interested in the following files.

- `app.py`: Build an AOT module and save the compilation artifacts (to the `module` directory);
- `app.cpp`: Load an AOT module, allocate resources and launch kernels;
- `CMakeLists.txt`: Find Taichi C-API library and link it to your project.

## Build for Whatever

1. Setup environment variables:

|Key|Value|
|-|-|
|`TAICHI_C_API_INSTALL_DIR`|CMake install directory. Something like `/path/to/taichi/_skbuild/win-amd64-3.9/cmake-install/c_api`.|

2. Generate AOT module with:

```bash
python app.py
```

3. Build this repo with:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
