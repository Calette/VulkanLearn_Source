#include "ConstantBuffer.h"
#include "Render/Vulkan/VulkanCommon.h"
#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
	using PaletteGlobal::device;
	VkConstantBuffer::VkConstantBuffer(uint64_t bufferSize, std::string name, unsigned binding, ConstantBufferType type)
		: m_Name(name), m_Binding(binding), m_BufferSize(bufferSize), m_Type(type)
	{
		_CreateUniformBuffers();
		_CreateDescriptorPool();
	}

	VkConstantBuffer::~VkConstantBuffer()
	{
		for (size_t i = 0; i < m_UniformBuffers.size(); i++)
		{
			vkDestroyBuffer(device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(device, m_UniformBuffersMemory[i], nullptr);
		}

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	}

	VkDescriptorSet& VkConstantBuffer::GetDescriptorSet()
	{
		return m_DescriptorSets[PaletteGlobal::vulkanDevice->GetImageIndex()];
	}

	void VkConstantBuffer::UpdateUniformBuffer()
	{
		uint32_t index = PaletteGlobal::vulkanDevice->GetImageIndex();
		void* data;
		ConstantBuffer cb;
		switch (m_Type)
		{
		case globalConstant:
			cb = GlobalConstantBuffer::Instance()->GetGlobalConstantBuffer(globalConstant);

			VK_CHECK_RESULT(vkMapMemory(device, m_UniformBuffersMemory[index], 0, sizeof(cb), 0, &data))
				memcpy(data, &cb, sizeof(cb));
			vkUnmapMemory(device, m_UniformBuffersMemory[index]);
			break;
		default:
			uint32_t index = PaletteGlobal::vulkanDevice->GetImageIndex();

			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			VK_CHECK_RESULT(vkMapMemory(device, m_UniformBuffersMemory[index], 0, sizeof(model), 0, &data))
				memcpy(data, &model, sizeof(model));
			vkUnmapMemory(device, m_UniformBuffersMemory[index]);
			break;
		}
	}

	void VkConstantBuffer::_CreateUniformBuffers()
	{
		size_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		auto& uniformBuffers = m_UniformBuffers;
		auto& uniformBuffersMemory = m_UniformBuffersMemory;
		uniformBuffers.resize(size);
		uniformBuffersMemory.resize(size);

		for (size_t i = 0; i < size; i++)
		{
			CreateBuffer(m_BufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
		}
	}

	void VkConstantBuffer::_CreateDescriptorPool()
	{
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		size *= 2;
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

	void VkConstantBuffer::CreateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout)
	{
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		std::vector<VkDescriptorSetLayout> layouts(size, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = size;
		allocInfo.pSetLayouts = layouts.data();	

		m_DescriptorSets.resize(size);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data()))

		for (size_t i = 0; i < size; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = m_BufferSize;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = m_Binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	GlobalConstantBuffer::GlobalConstantBuffer()
	{

	}

	ConstantBuffer GlobalConstantBuffer::GetGlobalConstantBuffer(ConstantBufferType cbtype)
	{
		uint32_t type = (uint32_t)cbtype;
		if (type & (uint32_t)ConstantBufferType::globalConstant)
		{
			return m_GlobalConstant;
		}
		return ConstantBuffer{};
	}

	ConstantBufferType GlobalConstantBuffer::GetConstantBufferType(const std::string& name)
	{
		if (name.compare("globalUniformBuffer"))
			return globalConstant;
		else
			return customConstant;
	}

	void GlobalConstantBuffer::UpdateUniformBuffer()
	{
		uint32_t index = PaletteGlobal::vulkanDevice->GetImageIndex();
		auto& extent = PaletteGlobal::vulkanDevice->GetExtent2D();

		m_GlobalConstant.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		m_GlobalConstant.proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
		m_GlobalConstant.proj[1][1] *= -1;
	}
}