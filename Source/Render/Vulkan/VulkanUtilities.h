#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
}
