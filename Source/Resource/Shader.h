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

namespace Palette
{
	class IShaderModuleResourse : public IRef
	{
	public:
		std::string GetName() const noexcept { return m_Name; }
		uint32_t PassFlag() const noexcept { return m_PassFlag; }

		virtual VkPipelineShaderStageCreateInfo* GetShaderStages() = 0;

	protected:
		virtual ~IShaderModuleResourse() {};
		IShaderModuleResourse();

		void _CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& code);

	protected:
		std::string m_Name;
		uint32_t m_PassFlag = 0u;
	};

	class VertexFragShaderModule : public IShaderModuleResourse
	{
		friend class ShaderResource;

	public:
		~VertexFragShaderModule() override;
		void OnRefDestroy() override;
		VkPipelineShaderStageCreateInfo* GetShaderStages() override { return m_ShaderStages; }

	protected:
		VertexFragShaderModule(const std::string& path, const std::string& name);

		void _CreateShaderInfo(std::vector<std::uint32_t>& vert_spirv, std::vector<std::uint32_t>& frag_spirv);
		void _CreatePipelineShaderStage();

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
		~ComputeShaderModule() override;
		ComputeShaderModule(const std::string& path, const std::string& name);

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

		IShaderModule GetShaderModule();

		VkPipeline& GetPipeline() { return m_Pipeline; }

		std::string& GetName() { return m_Name; }

		std::string& GetSourcePath() { return m_SourcePath; }

		PassType GetPassType() { return m_PassType; }

		bool HasPipeline() { return m_HasPipeline; }

		void FinishCreatePipeline() { m_HasPipeline = true; }

		void RealeaseShaderModule();

	protected:
		void _ReloadShader(uint32_t newTimeStamp);

	protected:
		std::string						m_Name;
		// different define / passFlag
		IShaderModule					m_ShaderModules;

		static const std::string		DEFUALTSHADERPATH;
		static Shader					defaultShader;

		bool							m_HasPipeline	= false;
		PassType						m_PassType;
		VkPipeline						m_Pipeline;
		std::string						m_SourcePath;
		ShaderType						m_Type;
		uint32_t						m_TimeStamp;
		VertexFormat					m_VertexFormat;
		std::vector<ShaderParameter>	m_Parameters;
	};
}
