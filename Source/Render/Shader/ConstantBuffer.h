#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <vector>
#include <string>

#include "Common/Singleton.h"
#include "ShaderDeclaration.h"
#include "Render/Vulkan/VulkanUtilities.h"
#include "Common/PaletteCommon.h"

namespace Palette
{
	class VkConstantBuffer
	{
	public:
		VkConstantBuffer() = default;
		~VkConstantBuffer();
		VkConstantBuffer(VkDeviceSize bufferSize, std::string name, unsigned set, unsigned binding, ConstantBufferType type);

		void UpdateUniformBuffer();
		void CreateDescriptorSet(std::vector<VkDescriptorSet>& descriptorSets);
		void SetData(uint32_t offset, void* data, size_t size);

		GET(std::string, Name)
		GET(unsigned, Set)
		GET(unsigned, Binding)
		GET(VkDeviceSize, BufferSize)
		GET(ConstantBufferType, Type)
		GETP(ConstantBuffer, ConstantBuffer)

	protected:
		void _CreateUniformBuffers();

	protected:
		std::vector<VkBuffer>		m_UniformBuffers;
		std::vector<VkDeviceMemory>	m_UniformBuffersMemory;
	};

	class GlobalConstantBuffer : public Singleton<GlobalConstantBuffer>
	{
	public:
		GlobalConstantBuffer();
		~GlobalConstantBuffer() = default;

		void ReleaseGlobalConstant();
		ConstantBufferType GetConstantBufferType(const std::string& name);

		void UpdateUniformBuffer();

		GETP(VkConstantBuffer, GlobalConstant)
		GET(unsigned, Set)
		GET(VkDescriptorSetLayout, DescriptorSetLayout)
		GET(std::vector<VkDescriptorSet>, DescriptorSets)

	protected:
		VkDescriptorPool	m_DescriptorPool;
	};
}