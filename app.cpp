#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <taichi/cpp/taichi.hpp>

using namespace std::chrono_literals;

const uint32_t WIDTH = 64;
const uint32_t HEIGHT = 32;

void clear_screen() {
#if defined(_WIN32)
  system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
  system("clear");
#elif defined(__APPLE__)
  system("clear");
#endif
}
void draw(TiRuntime runtime, const TiNdArray& canvas) {
  clear_screen();

  auto canvas_data = (const float*)ti_map_memory(runtime, canvas.memory);
  for (uint32_t y = 0; y < HEIGHT; ++y) {
    for (uint32_t x = 0; x < WIDTH; ++x) {
      float value = canvas_data[y * WIDTH + x];
      std::cout << (value > 0.5f ? '#' : ' ');
    }
    std::cout << std::endl;
  }
  ti_unmap_memory(runtime, canvas.memory);
}

int main(int argc, const char** argv) {
  TiRuntime runtime = ti_create_runtime(TI_ARCH_VULKAN);

  TiAotModule aot_module = ti_load_aot_module(runtime, "module");
  TiComputeGraph fractal = ti_get_aot_module_compute_graph(aot_module, "fractal");

  TiMemoryAllocateInfo mai {};
  mai.host_read = true;
  mai.size = WIDTH * HEIGHT * sizeof(float);
  TiMemory canvas_memory = ti_allocate_memory(runtime, &mai);

  TiNdArray canvas {};
  canvas.elem_type = TI_DATA_TYPE_F32;
  canvas.shape.dim_count = 2;
  canvas.shape.dims[0] = WIDTH;
  canvas.shape.dims[1] = HEIGHT;
  canvas.memory = canvas_memory;

  TiNamedArgument fractal_args[2];
  fractal_args[0].name = "canvas";
  fractal_args[0].argument.type = TI_ARGUMENT_TYPE_NDARRAY;
  fractal_args[0].argument.value.ndarray = canvas;

  for (uint32_t i = 0; i < 10000; ++i) {
    fractal_args[1].name = "t";
    fractal_args[1].argument.type = TI_ARGUMENT_TYPE_F32;
    fractal_args[1].argument.value.f32 = 0.03f * i;

    ti_launch_compute_graph(runtime, fractal, 2, fractal_args);

    ti_wait(runtime);

    draw(runtime, canvas);
    std::this_thread::sleep_for(0.03s);
  }

  ti_destroy_aot_module(aot_module);
  ti_destroy_runtime(runtime);

  return 0;
}
