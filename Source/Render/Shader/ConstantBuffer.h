#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <vector>

#include "Common/Singleton.h"
#include "ShaderDeclaration.h"
#include "Render/Vulkan/VulkanUtilities.h"

namespace Palette
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer();

		VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		VkDescriptorSet& GetDescriptorSet();

	protected:
		void _CreateDescriptorSetLayout();
		void _CreateUniformBuffers();
		void _CreateDescriptorPool();
		void _CreateDescriptorSets();

	protected:
		VkDescriptorPool m_DescriptorPool;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		std::vector<VkBuffer>		m_UniformBuffers;
		std::vector<VkDeviceMemory>	m_UniformBuffersMemory;
	};

	class GlobalConstantBuffer : public Singleton<GlobalConstantBuffer>, public ConstantBuffer
	{
	public:
		GlobalConstantBuffer();

		void GetGlobalConstantBuffer(ConstantBufferType cbtype, GlobalConstant* data);
		void ReleaseGlobalConstantBuffer();

		void UpdateUniformBuffer();
	};
}