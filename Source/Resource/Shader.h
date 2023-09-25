#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "Common/IRef.h"
#include "Common/IPtr.h"
#include "Resourse.h"
#include "Render/Shader/ShaderDeclaration.h"
#include "Render/Shader/ShaderParameter.h"
#include "Render/Mesh/VertexInputBindingDescription.h"
#include "Render/Shader/ConstantBuffer.h"
#include "Common/PaletteCommon.h"
#include "Render/Shader/ShaderCommon.h"
#include "Common/Math/Vector.h"

namespace Palette
{
	class IRenderPass;
	// 收集缓存没用到的shader变体？
	class ShaderModuleManager : Singleton<ShaderModuleManager>
	{

	};

	class IShaderModuleResourse : public IRef
	{
	public:
		std::string GetName() const noexcept { return m_Name; }
		uint32_t PassFlag() const noexcept { return m_PassFlag; }
		std::unordered_map<unsigned, std::vector<VkConstantBuffer*>>& GetConstantBuffers() noexcept { return m_ConstantBuffers; }
		void GetDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets);
		void GetDescriptorSetLayouts(std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);

		virtual VkPipelineShaderStageCreateInfo* GetShaderStages() = 0;

	protected:
		virtual ~IShaderModuleResourse() = default;
		IShaderModuleResourse() = default;

		void _CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& code);
		void _CreateDescriptorSet();
		void _CreateDescriptorPool();
		void _InitParamters();

		GETD(ConstantBufferType, BufferType, CUSTOM_CONSTANT)
		GETP(ShaderResource, Shader)

	protected:
		std::string			m_Name;
		uint32_t			m_PassFlag = 0u;
		VkDescriptorPool	m_DescriptorPool;

		std::unordered_map<unsigned, VkDescriptorSetLayout>				m_DescriptorSetLayouts;
		std::unordered_map<unsigned, std::vector<VkDescriptorSet>>		m_DescriptorSets;
		std::unordered_map<unsigned, std::vector<VkConstantBuffer*>>	m_ConstantBuffers;
	};

	class VertexFragShaderModule : public IShaderModuleResourse
	{
		friend class ShaderResource;

	public:
		~VertexFragShaderModule() override = default;
		void OnRefDestroy() override;
		VkPipelineShaderStageCreateInfo* GetShaderStages() override { return m_ShaderStages; }

	protected:
		VertexFragShaderModule(const std::string& path, const std::string& name, ShaderResource* shader);

		void _CreateShaderInfo(std::vector<std::uint32_t>& vert_spirv, std::vector<std::uint32_t>& frag_spirv);
		void _CreatePipelineShaderStage();
		void _SetConstantBuffer();

	protected:
		std::string m_Vert_SPIR_V_Path;
		std::string m_Frag_SPIR_V_Path;

		std::string m_GLSL_Path;

		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
		VkPipelineShaderStageCreateInfo m_ShaderStages[2];
	};

	class ComputeShaderModule : public IShaderModuleResourse
	{
		friend class ShaderResource;

	public:
		VkPipelineShaderStageCreateInfo* GetShaderStages() override { return m_ShaderStages; }
		void OnRefDestroy() override;

	protected:
		~ComputeShaderModule() override = default;
		ComputeShaderModule(const std::string& path, const std::string& name, ShaderResource* shader);

		void _CreatePipelineShaderStage(VkShaderModule computeShaderModule);

	protected:
		std::string m_Compute_SPIR_V_Path;
		VkPipelineShaderStageCreateInfo* m_ShaderStages = nullptr;
	};

	class ShaderResource : public IRef
	{
	public:
		~ShaderResource();
		ShaderResource(const std::string& path);

		static Shader GetDefaultShader();
		static void ReleaseDefaultShader();

		IShaderModule GetShaderModule();

		void CreatePipeline(IRenderPass* pass);

		void PrepareRendering(IRenderPass* pass);

		void RealeaseShaderModule();

		SETSHADERPARAM(Float, float)
		SETSHADERPARAM(Vector, Vector4)
		SETSHADERPARAM(Vector, Vector3)
		SETSHADERPARAM(Int, uint64_t)
		SETSHADERPARAM(Matrix4, glm::mat4)

		typedef std::unordered_map<std::string, ShaderParameter> map_string_ShaderParameter;
		GET(map_string_ShaderParameter, Parameters)
		GET(VkPipelineLayout, PipelineLayout)
		GET(VkPipeline, Pipeline)
		GET(std::string, Name)
		GET(std::string, SourcePath)
		GET(PassType, PassType)

	protected:
		void _ReloadShader(uint32_t newTimeStamp);

	protected:
		// different define / passFlag
		IShaderModule					m_ShaderModule;

		static const std::string		DEFAULTSHADERPATH;
	public:
		static Shader					defaultShader;

		bool							m_HasPipeline	= false;
		ShaderType						m_Type;
		uint32_t						m_TimeStamp;
		VertexFormat					m_VertexFormat;
	};
}
