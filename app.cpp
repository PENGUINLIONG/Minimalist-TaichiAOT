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
void draw(const ti::NdArray<float>& canvas) {
  clear_screen();

  auto canvas_data = (const float*)canvas.map();
  for (uint32_t y = 0; y < HEIGHT; ++y) {
    for (uint32_t x = 0; x < WIDTH; ++x) {
      float value = canvas_data[y * WIDTH + x];
      std::cout << (value > 0.5f ? '#' : ' ');
    }
    std::cout << std::endl;
  }
  canvas.unmap();
}

int main(int argc, const char** argv) {
  ti::Runtime runtime(TI_ARCH_VULKAN);

  ti::AotModule aot_module = runtime.load_aot_module("module");
  ti::ComputeGraph fractal = aot_module.get_compute_graph("fractal");

  ti::NdArray<float> canvas =
    runtime.allocate_ndarray<float>({ WIDTH, HEIGHT }, {}, true);

  fractal["canvas"] = canvas;

  for (uint32_t i = 0; i < 10000; ++i) {
    fractal["t"] = 0.03f * i;

    fractal.launch();
    runtime.wait();

    draw(canvas);
    std::this_thread::sleep_for(0.03s);
  }

  return 0;
}
