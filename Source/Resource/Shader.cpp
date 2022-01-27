#include <fstream>

#include "Shader.h"
#include "spirv_cross.hpp"
#include "../Common/FileSystem.h"
#include "../Common/MD5.h"
#include "../Render/Vulkan/VulkanGlobal.h"
#include "../Render/Mesh/VertexInputBindingDescription.h"
#include "Render/Vulkan/VulkanCommon.h"
#include "Render/Shader/ShaderCompiler.h"

namespace Palette
{
    IShaderModuleResourse::IShaderModuleResourse() 
    {

    }

    void IShaderModuleResourse::_CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VK_CHECK_RESULT(vkCreateShaderModule(PaletteGlobal::device, &createInfo, nullptr, &shaderModule))
    }

    VertexFragShaderModule::~VertexFragShaderModule()
    {

    }

    void VertexFragShaderModule::OnRefDestroy()
    {
        vkDestroyShaderModule(PaletteGlobal::device, m_VertShaderModule, nullptr);
        vkDestroyShaderModule(PaletteGlobal::device, m_FragShaderModule, nullptr);
    }

    static std::string GetConstantBufferStr(uint32_t binding, std::vector<char>& source)
    {
        std::string str = "";
        return str;
    }

    static void Parse(static std::vector<char>& buffer, static std::vector<char>& buffer_vert, static std::vector<char>& buffer_frag)
    {
        const std::string versionStr = "#version 460\n";

        std::string mainStr = "";

        std::string str;

        buffer_vert = std::vector<char>(str.begin(), str.end());
        buffer_frag = std::vector<char>(str.begin(), str.end());
    }

    VertexFragShaderModule::VertexFragShaderModule(const std::string& path, const std::string& name)
        : IShaderModuleResourse()
    {
        std::string m_Vert_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".vert";
        std::string m_Frag_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".frag";

        m_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".shader";

        // todo
        ////auto buffer = ReadFile(m_GLSL_Path);
        //Parse(buffer, buffer_vert, buffer_frag);

        auto buffer_vert = ReadFile(m_Vert_GLSL_Path);
        auto buffer_frag = ReadFile(m_Frag_GLSL_Path);

        std::vector<std::uint32_t> vert_spirv;
        std::vector<std::uint32_t> frag_spirv;
        GLSLCompiler::Instance()->Load_Shader(buffer_vert, EShLangVertex, vert_spirv);
        GLSLCompiler::Instance()->Load_Shader(buffer_frag, EShLangFragment, frag_spirv);

        // todo cache
        m_Vert_SPIR_V_Path = GetShaderPath() + "Shaders/SPIR-V/" + name + "_vert.spv";
        m_Frag_SPIR_V_Path = GetShaderPath() + "Shaders/SPIR-V/" + name + "_frag.spv";

        _CreateShaderModule(m_VertShaderModule, vert_spirv);
        _CreateShaderModule(m_FragShaderModule, frag_spirv);

        _CreatePipelineShaderStage();

        _CreateShaderInfo(vert_spirv, frag_spirv);
    }

    static void GetShaderResourcesInfo(std::vector<std::uint32_t>& sourceCode)
    {
        spirv_cross::Compiler glsl(sourceCode);
        // The SPIR-V is now parsed, and we can perform reflection on it.
        spirv_cross::ShaderResources resources = glsl.get_shader_resources();

        // Get all sampled images in the shader.
        for (auto& resource : resources.sampled_images)
        {

        }

        // Get all Uniform Buffer in the shader
        for (auto& resource : resources.uniform_buffers)
        {
            unsigned set = glsl.get_decoration(resource.id, spv::DecorationDescriptorSet);
            unsigned binding = glsl.get_decoration(resource.id, spv::DecorationBinding);

            spirv_cross::SPIRType uniformBufferType = glsl.get_type(resource.type_id);
            const std::string& varName = glsl.get_name(resource.id);
            unsigned uniformBufferStructSize = glsl.get_declared_struct_size(uniformBufferType);

            uint32_t member_count = uniformBufferType.member_types.size();
            for (int i = 0; i < member_count; i++)
            {
                std::cout<<glsl.get_member_name(resource.base_type_id, i)<<std::endl;
            }
        }

        // Get all sampled images in the shader.
        for (auto& resource : resources.storage_buffers)
        {

        }

        for (auto& resource : resources.storage_images)
        {

        }
    }

    void VertexFragShaderModule::_CreateShaderInfo(std::vector<std::uint32_t>& vert_spirv, std::vector<std::uint32_t>& frag_spirv)
    {
        // todo get spirv_cross::ShaderResources
        try 
        {
            GetShaderResourcesInfo(vert_spirv);
        }
        catch (const spirv_cross::CompilerError& e) 
        {
            std::cout << (e.what());
            abort();
        }
    }

    void VertexFragShaderModule::_CreatePipelineShaderStage()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = m_VertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = m_FragShaderModule;
        fragShaderStageInfo.pName = "main";

        m_ShaderStages[0] = vertShaderStageInfo;
        m_ShaderStages[1] = fragShaderStageInfo;
    }

    ComputeShaderModule::~ComputeShaderModule()
    {

    }

    ComputeShaderModule::ComputeShaderModule(const std::string& path, const std::string& name)
        : IShaderModuleResourse()
    {
        
    }

    void ComputeShaderModule::OnRefDestroy()
    {

    }

    void ComputeShaderModule::_CreatePipelineShaderStage(VkShaderModule computeShaderModule)
    {
        
    }

    const std::string ShaderResource::DEFUALTSHADERPATH = "triangle.shader";
    Shader ShaderResource::defaultShader = nullptr;

    ShaderResource::~ShaderResource()
    {
        vkDestroyPipeline(PaletteGlobal::device, m_Pipeline, nullptr);
    }

    ShaderResource::ShaderResource(const std::string& path)
        : m_SourcePath(path)
    {
        size_t namePos = path.find_last_of('/');
        size_t extPos = path.find_last_of('.');

        m_Name = path.substr(namePos + 1, extPos);
        std::string ext = path.substr(extPos + 1);

        if (ext.compare("shader") == 0)
        {
            m_Type = ShaderType::VertexFrag;
            try
            {
                m_ShaderModules = IShaderModule(new VertexFragShaderModule(path, m_Name));
            }
            catch (const std::exception& e)
            {
                m_Type = ShaderType::None;
                printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
            }
        }
        else if (ext.compare("compute") == 0)
        {
            m_Type = ShaderType::Compute;
            try
            {
                m_ShaderModules = IShaderModule(new ComputeShaderModule(path, m_Name));
            }
            catch (const std::exception& e)
            {
                m_Type = ShaderType::None;
                printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
            }
        }
        else
        {
            m_Type = ShaderType::None;
        }

        // tempCode
        m_PassType = PassType::SimplePass;
        //m_Parameters.push_back(ShaderParameter{})
    }

    Shader ShaderResource::GetDefaultShader()
    {
        if (!defaultShader)
        {
            defaultShader = Shader(new ShaderResource(DEFUALTSHADERPATH));
        }
        return defaultShader;
    }

    IShaderModule ShaderResource::GetShaderModule()
    {
        if (m_Type == ShaderType::None)
        {
            auto shader = GetDefaultShader();
            if (shader->m_Type != ShaderType::None)
                return shader->GetShaderModule();
            else
                throw std::runtime_error("defaultShader fails to load");
        }
        return m_ShaderModules;
    }

    void ShaderResource::RealeaseShaderModule()
    {
        //m_ShaderModules->ReleaseRef();
    }

    void ShaderResource::_ReloadShader(uint32_t newTimeStamp)
    {
        if (newTimeStamp == m_TimeStamp)
            return;
        m_TimeStamp = newTimeStamp;

        //...
    }

}
