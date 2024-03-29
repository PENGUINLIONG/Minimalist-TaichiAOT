cmake_minimum_required(VERSION 3.13)

set(TAICHI_AOT_APP_NAME TaichiAot)

project(${TAICHI_AOT_APP_NAME} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT EXISTS ${TAICHI_C_API_INSTALL_DIR})
    set(${TAICHI_C_API_INSTALL_DIR} "$ENV{TAICHI_C_API_INSTALL_DIR}")
endif()

set(TAICHI_C_API_INSTALL_DIR ${TAICHI_C_API_INSTALL_DIR} CACHE PATH "Taichi C-API installation directory")

# Check for `TAICHI_C_API_INSTALL_DIR`.
get_filename_component(TAICHI_C_API_INSTALL_DIR "${TAICHI_C_API_INSTALL_DIR}" ABSOLUTE)
if (NOT EXISTS ${TAICHI_C_API_INSTALL_DIR})
    message(FATAL_ERROR "TAICHI_C_API_INSTALL_DIR is not specified")
endif()
message("-- TAICHI_C_API_INSTALL_DIR=" ${TAICHI_C_API_INSTALL_DIR})

# Declare executable target.
add_executable(${TAICHI_AOT_APP_NAME} app.cpp)
target_include_directories(${TAICHI_AOT_APP_NAME} PUBLIC ${TAICHI_C_API_INSTALL_DIR}/include)

# Find built taichi C-API library in `TAICHI_C_API_INSTALL_DIR`.
find_library(taichi_c_api taichi_c_api HINTS
    ${TAICHI_C_API_INSTALL_DIR}/lib
    # CMake find root is overriden by Android toolchain.
    NO_CMAKE_FIND_ROOT_PATH)
if (NOT EXISTS ${taichi_c_api})
    message(FATAL_ERROR "Couldn't find C-API library; ensure your Taichi is built with `TI_WITH_CAPI=ON`")
else()
    target_link_libraries(${TAICHI_AOT_APP_NAME} ${taichi_c_api})
endif()

# If you are building for Android, you need to link to system libraries.
if (ANDROID)
    find_library(android android)
    find_library(log log)
    target_link_libraries(${TAICHI_AOT_APP_NAME} android log)
endif()

# Copy Taichi C-API dynamic library to build artifact directory.
if (WIN32)
    file(GLOB taichi_c_api_SRC "${TAICHI_C_API_INSTALL_DIR}/bin")
    string(REPLACE "${TAICHI_C_API_INSTALL_DIR}/bin" "" taichi_c_api_DST ${taichi_c_api_SRC})
    add_custom_command(
        TARGET ${TAICHI_AOT_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${taichi_c_api_SRC} $<TARGET_FILE_DIR:${TAICHI_AOT_APP_NAME}>/${taichi_c_api_DST}
        VERBATIM)
else()
    set(taichi_c_api_SRC ${taichi_c_api})
    string(REPLACE "${TAICHI_C_API_INSTALL_DIR}/lib" "" taichi_c_api_DST ${taichi_c_api_SRC})
    add_custom_command(
        TARGET ${TAICHI_AOT_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${taichi_c_api_SRC} $<TARGET_FILE_DIR:${TAICHI_AOT_APP_NAME}>/${taichi_c_api_DST}
        VERBATIM)
endif()

# MoltenVK dylib should be copied to the output directory, otherwise Taichi
# complains that it couldn't find `TI_LIB_DIR`. You should specify the
# environment directory`TI_LIB_DIR` launching on an Apple OS. Please refer to
# `scripts\run-macos.sh` for detail.
if (APPLE)
    find_library(MoltenVK libMoltenVK.dylib PATHS $HOMEBREW_CELLAR/molten-vk $VULKAN_SDK REQUIRED)
    add_custom_command(
        TARGET ${TAICHI_AOT_APP_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND}
        ARGS -E copy ${MoltenVK} $<TARGET_FILE_DIR:${TAICHI_AOT_APP_NAME}>/libMoltenVK.dylib
        VERBATIM)
endif()
