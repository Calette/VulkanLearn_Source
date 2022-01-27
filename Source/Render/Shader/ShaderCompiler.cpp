#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "ShaderCompiler.h"
#include "Common/FileSystem.h"
#include <StandAlone/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

namespace Palette
{
	glslang::EShTargetLanguage        GLSLCompiler::env_target_language = glslang::EShTargetLanguage::EShTargetNone;
	glslang::EShTargetLanguageVersion GLSLCompiler::env_target_language_version = (glslang::EShTargetLanguageVersion)0;

	static EShLanguage Find_shader_stage(const std::string& ext)
	{
		if (ext == "vert")
		{
			return EShLangVertex;
		}
		else if (ext == "frag")
		{
			return EShLangFragment;
		}
		else if (ext == "comp")
		{
			return EShLangCompute;
		}
		else if (ext == "geom")
		{
			return EShLangGeometry;
		}
		else if (ext == "tesc")
		{
			return EShLangTessControl;
		}
		else if (ext == "tese")
		{
			return EShLangTessEvaluation;
		}
		else if (ext == "rgen")
		{
			return EShLangRayGen;
		}
		else if (ext == "rahit")
		{
			return EShLangAnyHit;
		}
		else if (ext == "rchit")
		{
			return EShLangClosestHit;
		}
		else if (ext == "rmiss")
		{
			return EShLangMiss;
		}
		else if (ext == "rint")
		{
			return EShLangIntersect;
		}
		else if (ext == "rcall")
		{
			return EShLangCallable;
		}

		throw std::runtime_error("File extension `" + ext + "` does not have a vulkan shader stage.");
	}

	void GLSLCompiler::Set_target_environment(glslang::EShTargetLanguage target_language, glslang::EShTargetLanguageVersion target_language_version)
	{
		GLSLCompiler::env_target_language = target_language;
		GLSLCompiler::env_target_language_version = target_language_version;
	}
	
	void GLSLCompiler::Reset_target_environment()
	{
		GLSLCompiler::env_target_language = glslang::EShTargetLanguage::EShTargetNone;
		GLSLCompiler::env_target_language_version = (glslang::EShTargetLanguageVersion)0;
	}

	bool GLSLCompiler::Compile_to_spirv(
		EShLanguage language,
		const std::vector<char>& glsl_source,
		const std::string& entry_point,
		std::vector<std::uint32_t>& spirv,
		std::string& info_log)
	{
		// Initialize glslang library.
		glslang::InitializeProcess();

		EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);

		std::string source = std::string(glsl_source.begin(), glsl_source.end());

		const char* file_name_list[1] = { "" };
		const char* shader_source = reinterpret_cast<const char*>(source.data());

		glslang::TShader shader(language);
		shader.setStringsWithLengthsAndNames(&shader_source, nullptr, file_name_list, 1);
		shader.setEntryPoint(entry_point.c_str());
		shader.setSourceEntryPoint(entry_point.c_str());

		// todo
		//shader.setPreamble(shader_variant.get_preamble().c_str());
		//shader.addProcesses(shader_variant.get_processes());
		shader.setPreamble("");
		const std::vector<std::string> p{};
		shader.addProcesses(p);

		if (GLSLCompiler::env_target_language != glslang::EShTargetLanguage::EShTargetNone)
		{
			shader.setEnvTarget(GLSLCompiler::env_target_language, GLSLCompiler::env_target_language_version);
		}

		if (!shader.parse(&glslang::DefaultTBuiltInResource, 100, false, messages))
		{
			info_log = std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog());
			return false;
		}

		// Add shader to new program object.
		glslang::TProgram program;
		program.addShader(&shader);

		// Link program.
		if (!program.link(messages))
		{
			info_log = std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
			return false;
		}

		// Save any info log that was generated.
		if (shader.getInfoLog())
		{
			info_log += std::string(shader.getInfoLog()) + "\n" + std::string(shader.getInfoDebugLog()) + "\n";
		}

		if (program.getInfoLog())
		{
			info_log += std::string(program.getInfoLog()) + "\n" + std::string(program.getInfoDebugLog());
		}

		glslang::TIntermediate* intermediate = program.getIntermediate(language);

		// Translate to SPIRV.
		if (!intermediate)
		{
			info_log += "Failed to get shared intermediate code.\n";
			return false;
		}

		spv::SpvBuildLogger logger;

		glslang::GlslangToSpv(*intermediate, spirv, &logger);

		info_log += logger.getAllMessages() + "\n";

		// Shutdown glslang library.
		glslang::FinalizeProcess();

		return true;
	}

	bool GLSLCompiler::Load_Shader(const std::vector<char>& buffer, EShLanguage language, std::vector<std::uint32_t>& spirv)
	{
		std::string info_log;
		// Compile the GLSL source
		if (!Compile_to_spirv(language, buffer, "main", spirv, info_log))
		{
			printf("Failed to compile shader , Error: %s", info_log.c_str());
			return false;
		}
	}
}