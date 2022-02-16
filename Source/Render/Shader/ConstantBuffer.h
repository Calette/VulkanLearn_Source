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
		virtual ~VkConstantBuffer();
		VkConstantBuffer(uint64_t bufferSize, std::string name, unsigned binding, ConstantBufferType type);

		VkDescriptorSet& GetDescriptorSet();
		virtual void UpdateUniformBuffer();
		void CreateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout);

		GET(std::string, Name)
		GET(unsigned, Binding)
		GET(uint64_t, BufferSize)
		GET(ConstantBufferType, Type)

	protected:
		void _CreateUniformBuffers();
		void _CreateDescriptorPool();

	protected:
		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		std::vector<VkBuffer>		m_UniformBuffers;
		std::vector<VkDeviceMemory>	m_UniformBuffersMemory;
	};

	class GlobalConstantBuffer : public Singleton<GlobalConstantBuffer>
	{
	public:
		GlobalConstantBuffer();
		~GlobalConstantBuffer() = default;

		ConstantBuffer GetGlobalConstantBuffer(ConstantBufferType cbtype);

		ConstantBufferType GetConstantBufferType(const std::string& name);

		void UpdateUniformBuffer();

	private:
		GlobalConstant m_GlobalConstant;
	};
}