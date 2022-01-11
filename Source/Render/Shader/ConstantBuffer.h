#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "Common/Singleton.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "ShaderDeclaration.h"
#include "World/Camera.h"
#include "Render/Vulkan/VulkanUtilities.h"

namespace Palette
{
	using PaletteGlobal::device;

	class GlobalConstantBuffer : public Singleton<GlobalConstantBuffer>
	{
	public:
		GlobalConstantBuffer();
		~GlobalConstantBuffer();

		void GetGlobalConstantBuffer(ConstantBufferType cbtype, GlobalConstant* data);

		void UpdateUniformBuffer();

		VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		VkDescriptorSet& GetDescriptorSet() { return m_DescriptorSets[PaletteGlobal::vulkanDevice->GetImageIndex()]; }

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
}