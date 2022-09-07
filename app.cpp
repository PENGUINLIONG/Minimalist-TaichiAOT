#include <iostream>
#include <taichi/cpp/taichi.hpp>

const uint32_t WIDTH = 16;
const uint32_t HEIGHT = 16;

int main(int argc, const char** argv) {
  ti::Runtime runtime(TI_ARCH_VULKAN);

  ti::AotModule aot_module = runtime.load_aot_module("module");
  ti::ComputeGraph g_run = aot_module.get_compute_graph("g_run");

  ti::NdArray<float> arr =
    runtime.allocate_ndarray<float>({ WIDTH, HEIGHT }, {}, true);

  g_run["arr"] = arr;
  g_run.launch();
  runtime.wait();

  std::vector<float> arr_data(WIDTH * HEIGHT);
  arr.read(arr_data);

  for (uint32_t y = 0; y < HEIGHT; ++y) {
    for (uint32_t x = 0; x < WIDTH; ++x) {
      float value = arr_data[y * WIDTH + x];
      std::cout << (value > 0.5f ? '#' : ' ');
    }
    std::cout << std::endl;
  }

  return 0;
}
