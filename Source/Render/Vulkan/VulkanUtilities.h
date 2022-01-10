#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        // The VkPhysicalDeviceMemoryProperties structure has two arrays memoryTypes and memoryHeaps.
        // Memory heaps are distinct memory resources like dedicated VRAM and swap space in RAM for when VRAM runs out.
        // The different types of memory exist within these heaps.
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(PaletteGlobal::physicalDevice, &memProperties);

        // typeFilter is used to specify the bit field of memory types that are suitable
        // The properties define special features of the memory, like being able to map it so we can write to it from the CPU. 
        // This property is indicated with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, 
        // but we also need to use the VK_MEMORY_PROPERTY_HOST_COHERENT_BIT property.
        // Unfortunately the driver may not immediately copy the data into the buffer memory, for example because of caching.
        // It is also possible that writes to the buffer are not visible in the mapped memory yet.
        // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ensures that the mapped memory always matches the contents of the allocated memory. 
        // Do keep in mind that this may lead to slightly worse performance than explicit flushing( that doesn't matter )
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(PaletteGlobal::device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(PaletteGlobal::device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(PaletteGlobal::device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(PaletteGlobal::device, buffer, bufferMemory, 0);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = PaletteGlobal::commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(PaletteGlobal::device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(PaletteGlobal::graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(PaletteGlobal::graphicsQueue);

        vkFreeCommandBuffers(PaletteGlobal::device, PaletteGlobal::commandPool, 1, &commandBuffer);
    }
}
