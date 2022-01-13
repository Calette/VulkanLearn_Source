#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Common/Singleton.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "Render/Vulkan/VulkanCommon.h"

namespace Palette
{
	class DescriptorPool : public Singleton<DescriptorPool>
	{
	public:
		DescriptorPool()
		{
			uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();

			VkDescriptorPoolSize poolSize{};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = size;

			VkDescriptorPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;
			poolInfo.maxSets = size;

			VK_CHECK_RESULT(vkCreateDescriptorPool(PaletteGlobal::device, &poolInfo, nullptr, &descriptorPool))
		}

		void ReleaseDescriptorPool()
		{
			vkDestroyDescriptorPool(PaletteGlobal::device, descriptorPool, nullptr);
		}

	protected:
		VkDescriptorPool descriptorPool;
	};
}