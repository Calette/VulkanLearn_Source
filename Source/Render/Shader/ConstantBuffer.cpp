#include "ConstantBuffer.h"
#include "Render/Vulkan/VulkanCommon.h"

namespace Palette
{
	GlobalConstantBuffer::GlobalConstantBuffer()
	{
		_CreateDescriptorSetLayout();
		_CreateUniformBuffers();
		_CreateDescriptorPool();
		_CreateDescriptorSets();
	}

	void GlobalConstantBuffer::ReleaseGlobalConstantBuffer()
	{
		for (size_t i = 0; i < m_UniformBuffers.size(); i++)
		{
			vkDestroyBuffer(device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(device, m_UniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	}

	void GlobalConstantBuffer::GetGlobalConstantBuffer(ConstantBufferType cbtype, GlobalConstant* data)
	{

	}

	void GlobalConstantBuffer::UpdateUniformBuffer()
	{
		uint32_t index = PaletteGlobal::vulkanDevice->GetImageIndex();
		auto& extent = PaletteGlobal::vulkanDevice->GetExtent2D();

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		GlobalConstant ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		void* data;
		VK_CHECK_RESULT(vkMapMemory(device, m_UniformBuffersMemory[index], 0, sizeof(ubo), 0, &data))
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, m_UniformBuffersMemory[index]);
	}

	void GlobalConstantBuffer::_CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout))
	}

	void GlobalConstantBuffer::_CreateUniformBuffers()
	{
		VkDeviceSize bufferSize = sizeof(GlobalConstant);

		size_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		auto& uniformBuffers = m_UniformBuffers;
		auto& uniformBuffersMemory = m_UniformBuffersMemory;
		uniformBuffers.resize(size);
		uniformBuffersMemory.resize(size);

		for (size_t i = 0; i < size; i++)
		{
			CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
		}
	}

	void GlobalConstantBuffer::_CreateDescriptorPool()
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

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool))
	}

	void GlobalConstantBuffer::_CreateDescriptorSets()
	{
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();

		std::vector<VkDescriptorSetLayout> layouts(size, m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(size);
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(size);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data()))

		for (size_t i = 0; i < size; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(GlobalConstant);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
	}
}