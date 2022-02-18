#include "ConstantBuffer.h"
#include "Render/Vulkan/VulkanCommon.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "Common/Time.h"

namespace Palette
{
	using PaletteGlobal::device;
	VkConstantBuffer::VkConstantBuffer(VkDeviceSize bufferSize, std::string name, unsigned set, unsigned binding, ConstantBufferType type)
		: m_Name(name), m_Binding(binding), m_Set(set), m_BufferSize(bufferSize), m_Type(type)
	{
		switch (type)
		{
		case Palette::CUSTOM_CONSTANT:
			m_ConstantBuffer = new CustomConstant(bufferSize);
			break;
		case Palette::GLOBAL_CONSTANT:
			m_ConstantBuffer = new GlobalConstant();
			m_BufferSize = m_ConstantBuffer->GetSize();
			break;
		default:
			break;
		}

		_CreateUniformBuffers();
	}

	VkConstantBuffer::~VkConstantBuffer()
	{
		for (size_t i = 0; i < m_UniformBuffers.size(); i++)
		{
			vkDestroyBuffer(device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(device, m_UniformBuffersMemory[i], nullptr);
		}
	}

	void VkConstantBuffer::UpdateUniformBuffer()
	{
		VkDeviceSize size = m_ConstantBuffer->GetSize();
		void* cb = m_ConstantBuffer->GetData();
			
		uint32_t index = PaletteGlobal::vulkanDevice->GetImageIndex();
		void* data;
		VK_CHECK_RESULT(vkMapMemory(device, m_UniformBuffersMemory[index], 0, size, 0, &data));
		memcpy(data, cb, size);
		vkUnmapMemory(device, m_UniformBuffersMemory[index]);
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

	void VkConstantBuffer::SetData(uint32_t offset, void* data, size_t size)
	{
		memcpy(m_ConstantBuffer->GetData(), data, size);
	}

	void VkConstantBuffer::CreateDescriptorSet(std::vector<VkDescriptorSet>& descriptorSets)
	{
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();

		for (size_t i = 0; i < size; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = m_BufferSize;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptorSets[i];
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
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		m_Set = 0;
		m_GlobalConstant = new VkConstantBuffer(0, "globalUniformBuffer", 0, 0, GLOBAL_CONSTANT);

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		// buffer array length
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout))

		VkDescriptorPoolSize uniformPoolSize{};
		uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformPoolSize.descriptorCount = size;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &uniformPoolSize;
		// tempCode
		poolInfo.maxSets = 512;

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool))

		std::vector<VkDescriptorSetLayout> layouts(size, m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = size;
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(size);
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data()))

		m_GlobalConstant->CreateDescriptorSet(m_DescriptorSets);
	}

	void GlobalConstantBuffer::ReleaseGlobalConstant()
	{
		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		delete m_GlobalConstant;
	}

	ConstantBufferType GlobalConstantBuffer::GetConstantBufferType(const std::string& name)
	{
		if (name.compare("globalUniformBuffer") == 0)
			return GLOBAL_CONSTANT;
		else
			return CUSTOM_CONSTANT;
	}

	void GlobalConstantBuffer::UpdateUniformBuffer()
	{
		auto& extent = PaletteGlobal::vulkanDevice->GetExtent2D();

		GlobalConstant* globalConstant = dynamic_cast<GlobalConstant*>(m_GlobalConstant->GetConstantBuffer());
		globalConstant->view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		globalConstant->proj = glm::perspective(glm::radians(45.0f), extent.width / (float)extent.height, 0.1f, 10.0f);
		globalConstant->proj[1][1] *= -1;

		m_GlobalConstant->UpdateUniformBuffer();
	}
}