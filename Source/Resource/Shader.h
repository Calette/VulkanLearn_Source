#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "Common/IRef.h"
#include "Common/IPtr.h"

namespace Palette
{
	enum class ShaderType
	{
		VertexPixel,
		Compute,
		None
	};

	class IShaderModule : public IRef
	{
	public:
		std::string GetName() const noexcept { return m_Name; }
		uint32_t PassFlag() const noexcept { return m_PassFlag; }

		virtual VkPipelineShaderStageCreateInfo* GetShaderStages() = 0;

	protected:
		virtual ~IShaderModule() {};
		IShaderModule(const std::string& path);

		void _CreateShaderModule(VkShaderModule& shaderModule, const std::vector<char>& code);

	protected:
		std::string m_Name;
		uint32_t m_PassFlag;
	};

	class VertexPixelShaderModule : public IShaderModule
	{
		friend class Shader;

	public:
		~VertexPixelShaderModule() override;
		void OnRefDestroy() override;
		VkPipelineShaderStageCreateInfo* GetShaderStages() override { return m_ShaderStages; }

	protected:
		VertexPixelShaderModule(const std::string& path, const std::string& name);

		void _CreatePipelineShaderStage();

	protected:
		std::string m_Vert_SPIR_V_Path;
		std::string m_Pixel_SPIR_V_Path;

		VkShaderModule m_VertShaderModule;
		VkShaderModule m_PixelShaderModule;
		VkPipelineShaderStageCreateInfo m_ShaderStages[2];
	};

	class ComputeShaderModule : public IShaderModule
	{
		friend class Shader;

	public:
		VkPipelineShaderStageCreateInfo* GetShaderStages() override { return m_ShaderStages; }
		void OnRefDestroy() override;

	protected:
		~ComputeShaderModule() override;
		ComputeShaderModule(const std::string& path, const std::string& name);

		void _CreatePipelineShaderStage(VkShaderModule* computeShaderModule);

	protected:
		std::string m_Compute_SPIR_V_Path;
		VkPipelineShaderStageCreateInfo* m_ShaderStages = nullptr;
	};

	class Shader
	{
	public:
		~Shader();
		Shader(const std::string& path);

		static Shader* GetDefaultShader();

		TSharedPtr<IShaderModule> GetShaderModule(uint32_t defineHash);

		void RealeaseAllShaderModule();

		void RealeaseShaderModule(uint32_t defineHash);

	protected:
		void _ReloadShader(uint32_t newTimeStamp);

	protected:
		std::string m_Name;
		// different define
		std::unordered_map<uint32_t, TSharedPtr<IShaderModule>> m_ShaderModules;

		static const std::string defualtShaderPath;
		static Shader* defaultShader;

		std::string m_SourcePath;
		ShaderType m_Type;
		uint32_t m_TimeStamp;
	};
}
