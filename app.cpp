#include <cstdio>
#include <utility>
#include <string>
#include <vulkan/vulkan.h>
#include "taichi/taichi_core.h"
#include "taichi/taichi_vulkan.h"

#define MAP_MEMORY 1;

static int FindMemoryByFlagAndTypeNew(VkMemoryPropertyFlagBits memoryFlagBits, uint32_t  memoryTypeBits, VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceMemoryProperties	vpdmp;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &vpdmp);
	for (unsigned int i = 0; i < vpdmp.memoryTypeCount; i++)
	{
		VkMemoryType vmt = vpdmp.memoryTypes[i];
		VkMemoryPropertyFlags vmpf = vmt.propertyFlags;
		if ((memoryTypeBits & (1 << i)) != 0)
		{
			if ((vmpf & memoryFlagBits) != 0)
			{
				return i;
			}
		}
	}
	throw  std::runtime_error("Could not find given memory flag and type");
}

int main()
{
  printf("----taichi_aot_import_mem test---\n");
  //create_vulkan_runtime
  TiRuntime runtime = ti_create_runtime(TI_ARCH_VULKAN);
  TiVulkanRuntimeInteropInfo export_runtime_info;
  ti_export_vulkan_runtime(runtime, &export_runtime_info);
  printf("-----runtime = %p ----------\n", runtime);
  std::string aot_dir = "module";
  std::string func_name = "taichi_add";

  // load aot and kernel
  TiAotModule aot_mod = ti_load_aot_module(runtime, aot_dir.c_str());
  if (aot_mod == nullptr) {
    printf("load aot module failed! \n");
    return  -1;
  }
  TiComputeGraph g = ti_get_aot_module_compute_graph(aot_mod, func_name.c_str());
  if (g == nullptr) {
    printf("load aot module failed! \n");
    return -1;
  }
  printf("aot_mod = %p  g = %p \n", aot_mod, g);

  VkDevice device = export_runtime_info.device;
  TiMemory in_ten_mem, out_ten_mem;
  TiArgumentValue in_ten, out_ten;
  TiDataType data_type = TiDataType::TI_DATA_TYPE_F32;

  const int width = 320;
  const int height = 320;
  const int ch = 4;
  const int byte_size = width * height * ch * sizeof(float);
  printf("byte_size = %d \n", byte_size);

  //create buffer input
  VkBufferCreateInfo bufferInfo;
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.pNext = nullptr;
  bufferInfo.flags = 0;
  bufferInfo.size = byte_size;
  bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
  	VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  	VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  //bufferInfo.usage = 131107;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferInfo.queueFamilyIndexCount = 0;
  bufferInfo.pQueueFamilyIndices = nullptr;

  VkBuffer bufferIn;
  PFN_vkCreateBuffer vkCreateBuffer_ = (PFN_vkCreateBuffer)export_runtime_info.get_instance_proc_addr(export_runtime_info.instance, "vkCreateBuffer");
  VkResult res = vkCreateBuffer_(device, &bufferInfo, nullptr, &bufferIn);
  if (res != VK_SUCCESS) {
    printf("vkCreateBuffer failed! res = %d \n", res);
    return -1;
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(device, bufferIn, &memRequirements);
  uint32_t  memoryTypeIndex;
  memoryTypeIndex = FindMemoryByFlagAndTypeNew(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, memRequirements.memoryTypeBits, export_runtime_info.physical_device);
  printf("memoryTypeIndex = %d \n", memoryTypeIndex);

  //create memory input
  printf("memRequirements.size = %d \n", memRequirements.size);
  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = memoryTypeIndex;
  VkDeviceMemory vkmemoryIn;
  res = vkAllocateMemory(device, &allocInfo, nullptr, &vkmemoryIn);
  if (res != VK_SUCCESS) {
    printf("vkAllocateMemory failed! res = %d \n", res);
    return -1;
  }

  //map memory
  float* payload;
  vkMapMemory(device, vkmemoryIn, 0, byte_size, 0, (void**)&payload);
  for (int i = 0; i < width * height * ch; ++i) {
    payload[i] = i % 256;
  }
  vkUnmapMemory(device, vkmemoryIn);
  ti_wait(runtime);

  // bind memory 
  res = vkBindBufferMemory(device, bufferIn, vkmemoryIn, 0);
  if (res != VK_SUCCESS) {
    printf("vkBindBufferMemory input failed! res = %d \n", res);
    return -1;
  }
  else {
    printf("vkBindBufferMemory input success! res = %d \n", res);
  }

  //debug
  vkMapMemory(device, vkmemoryIn, 0, byte_size, 0, (void**)&payload);
  for (int i = 0; i < 16; ++i) {
    printf("input val = %f \n", payload[i]);
  }
  vkUnmapMemory(device, vkmemoryIn);
  ti_wait(runtime);

  //create buffer output
  VkBuffer bufferOut;
  res = vkCreateBuffer(device, &bufferInfo, nullptr, &bufferOut);
  if (res != VK_SUCCESS) {
    printf("vkCreateBuffer failed! res = %d \n", res);
    return -1;
  }

  VkDeviceMemory vkmemoryOut;
  res = vkAllocateMemory(device, &allocInfo, nullptr, &vkmemoryOut);
  if (res != VK_SUCCESS) {
    printf("vkAllocateMemory failed! res = %d \n", res);
    return -1;
  }
  res = vkBindBufferMemory(device, bufferOut, vkmemoryOut, 0);
  if (res != VK_SUCCESS) {
    printf("vkBindBufferMemory output failed! res = %d \n", res);
    return -1;
  }
  else {
    printf("vkBindBufferMemory output success! res = %d \n", res);
  }

  printf("00 vkmemoryOut = %p \n", vkmemoryOut);

  printf("------bufer in = %p buffer out = %p \n", bufferIn, bufferOut);
  TiVulkanMemoryInteropInfo mem_info;
  mem_info.buffer = bufferIn;
  mem_info.size = byte_size;
  //mem_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
  //	VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  //	VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  mem_info.usage = 131107;
  in_ten_mem = ti_import_vulkan_memory(runtime, &mem_info);
  if (in_ten_mem == nullptr) {
    printf("ti_import_vulkan_memory in mem failed! \n");
    return -1;
  }

  TiVulkanMemoryInteropInfo mem_info_out;
  mem_info_out.buffer = bufferOut;
  mem_info_out.size = byte_size;
  //mem_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
  //	VK_BUFFER_USAGE_TRANSFER_DST_BIT |
  //	VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  mem_info_out.usage = 131107;
  out_ten_mem = ti_import_vulkan_memory(runtime, &mem_info_out);
  if (out_ten_mem == nullptr) {
    printf("ti_import_vulkan_memory in mem failed! \n");
    return -1;
  }

  TiNdShape ti_shape;
  ti_shape.dim_count = 3;
  ti_shape.dims[0] = 320;
  ti_shape.dims[1] = 320;
  ti_shape.dims[2] = 4;
  TiNdArray arg_array_in;
  arg_array_in.memory = in_ten_mem;
  arg_array_in.shape = ti_shape;
  arg_array_in.elem_shape.dim_count = 0;
  arg_array_in.elem_type = data_type;
  in_ten.ndarray = std::move(arg_array_in);

  TiNdArray arg_array_out;
  arg_array_out.memory = out_ten_mem;
  arg_array_out.shape = ti_shape;
  arg_array_out.elem_shape.dim_count = 0;
  arg_array_out.elem_type = data_type;
  out_ten.ndarray = std::move(arg_array_out);
  ti_wait(runtime);

  printf("---in_ten_mem = %p  \n", in_ten_mem);
  printf("---out_ten_mem = %p \n", out_ten_mem);

  TiNamedArgument arg_0;
  arg_0.name = "in_ten";
  arg_0.argument.type = TiArgumentType::TI_ARGUMENT_TYPE_NDARRAY;
  arg_0.argument.value = std::move(in_ten);

  TiNamedArgument arg_1;
  arg_1.name = "out_ten";
  arg_1.argument.type = TiArgumentType::TI_ARGUMENT_TYPE_NDARRAY;
  arg_1.argument.value = std::move(out_ten);

  printf("---start ti_launch_compute_graph \n");
  constexpr uint32_t arg_count = 2;
  TiNamedArgument args[arg_count] = { std::move(arg_0), std::move(arg_1) };
  ti_launch_compute_graph(runtime, g, arg_count, &args[0]);
  ti_wait(runtime);
  printf("01 vkmemoryOut = %p \n", vkmemoryOut);

  //map memory
  vkMapMemory(device, vkmemoryOut, 0, byte_size, 0, (void**)&payload);
  for (int i = 0; i < 16; ++i) {
    printf("i = %d dst val = %f \n", i, payload[i]);
  }
  vkUnmapMemory(device, vkmemoryOut);

  vkDestroyBuffer(device, bufferIn, nullptr);
  vkDestroyBuffer(device, bufferOut, nullptr);
  vkFreeMemory(device, vkmemoryIn, nullptr);
  vkFreeMemory(device, vkmemoryOut, nullptr);

  ti_free_memory(runtime, in_ten_mem);
  ti_free_memory(runtime, out_ten_mem);

  ti_destroy_aot_module(aot_mod);
  ti_destroy_runtime(runtime);
  return 0;
}