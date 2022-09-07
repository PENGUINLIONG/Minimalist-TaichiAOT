cmake_minimum_required(VERSION 3.13)

set(TAICHI_AOT_APP_NAME TaichiAot)

project(${TAICHI_AOT_APP_NAME} LANGUAGES C CXX)

# Check for `TAICHI_C_API_INSTALL_DIR`.
set(TAICHI_C_API_INSTALL_DIR $ENV{TAICHI_C_API_INSTALL_DIR})
if (NOT EXISTS ${TAICHI_C_API_INSTALL_DIR})
message(FATAL_ERROR "Environment variable TAICHI_C_API_INSTALL_DIR is not specified")
endif()

# Find built taichi C-API library in `TAICHI_C_API_INSTALL_DIR`.
find_library(taichi_c_api taichi_c_api HINTS
    ${TAICHI_C_API_INSTALL_DIR}/lib
    ${TAICHI_C_API_INSTALL_DIR}/bin)
if (NOT EXISTS ${taichi_c_api})
message(FATAL_ERROR "Couldn't find C-API library; ensure your Taichi is built with `TI_WITH_CAPI=ON`")
endif()

# Executable target
add_executable(${TAICHI_AOT_APP_NAME} app.cpp)
target_include_directories(${TAICHI_AOT_APP_NAME} PUBLIC ${TAICHI_C_API_INSTALL_DIR}/include)
target_link_libraries(${TAICHI_AOT_APP_NAME} ${taichi_c_api})

# Copy Taichi C-API dynamic library to build artifact directory.
file(GLOB taichi_c_api_SRC "${TAICHI_C_API_INSTALL_DIR}/bin/taichi_c_api.*")
string(REPLACE "${TAICHI_C_API_INSTALL_DIR}/bin" "" taichi_c_api_DST ${taichi_c_api_SRC})
add_custom_command(TARGET ${TAICHI_AOT_APP_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy ${taichi_c_api_SRC} $<TARGET_FILE_DIR:${TAICHI_AOT_APP_NAME}>/${taichi_c_api_DST})
