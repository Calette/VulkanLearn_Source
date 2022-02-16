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
    using PaletteGlobal::device;

    void IShaderModuleResourse::_CreateShaderModule(VkShaderModule& shaderModule, const std::vector<uint32_t>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size() * sizeof(uint32_t);
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))
    }

    void IShaderModuleResourse::_CreateDescriptorSet()
    {
        for (auto cb : m_ConstantBuffers)
        {
            cb->CreateDescriptorSet(m_DescriptorSetLayout);
        }
    }

    void VertexFragShaderModule::OnRefDestroy()
    {
        vkDestroyShaderModule(device, m_VertShaderModule, nullptr);
        vkDestroyShaderModule(device, m_FragShaderModule, nullptr);
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
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

        _CreateDescriptorSet();
    }

    static ShaderParameterType SPIRTypeToShaderParameterType(spirv_cross::SPIRType spirType, size_t size)
    {
        switch (spirType.basetype)
        {
        case spirv_cross::SPIRType::Boolean:
            return ShaderParameterType::Boolean;
        case spirv_cross::SPIRType::Int:
            return ShaderParameterType::Integer;
        case spirv_cross::SPIRType::Float:
            switch (size)
            {
            case 4:
                return ShaderParameterType::Float;
            case 8:
                return ShaderParameterType::Vector2;
            case 12:
                return ShaderParameterType::Vector3;
            case 16:
                return ShaderParameterType::Vector4;
            case 64:
                return ShaderParameterType::Matrix4x4;
            default:
                return ShaderParameterType::UnKnown;
            }
        case spirv_cross::SPIRType::Image:
            return ShaderParameterType::Texture;
        default:
            return ShaderParameterType::UnKnown;
        }
    }

    static void GetShaderResourcesInfo(std::vector<std::uint32_t>& sourceCode, std::vector<VkDescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VertexFragShaderModule* shaderModule, VkShaderStageFlags stageFlags)
    {
        spirv_cross::Compiler compiler(sourceCode);
        // The SPIR-V is now parsed, and we can perform reflection on it.
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();

        auto& paramters = shaderModule->GetShaderParameters();
        auto& cbs = shaderModule->GetConstantBuffers();

        // Get all sampled images in the shader.
        for (auto& resource : resources.sampled_images)
        {

        }

        // Get all Uniform Buffer in the shader
        for (auto& resource : resources.uniform_buffers)
        {
            unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

            spirv_cross::SPIRType uniformBufferType = compiler.get_type(resource.type_id);
            const std::string& uniformBufferName = compiler.get_name(resource.id);
            size_t uniformBufferSize = compiler.get_declared_struct_size(uniformBufferType);
            printf("[%d]uniform_buffers : %s, size : %d\n", binding, uniformBufferName.c_str(), uniformBufferSize);
            unsigned uniformBufferStructSize = compiler.get_declared_struct_size(uniformBufferType);
            
            auto cbType = GlobalConstantBuffer::Instance()->GetConstantBufferType(uniformBufferName);

            uint32_t member_count = uniformBufferType.member_types.size();
            for (uint32_t i = 0; i < member_count; i++)
            {
                auto& memberName = compiler.get_member_name(resource.base_type_id, i);
                auto& memberType = compiler.get_type(uniformBufferType.member_types[i]);
                size_t memberSize = compiler.get_declared_struct_member_size(uniformBufferType, i);
                uint32_t offset = compiler.type_struct_member_offset(uniformBufferType, i);

                paramters.push_back(ShaderParameter{
                    memberName,
                    SPIRTypeToShaderParameterType(memberType, memberSize),
                    offset });
                printf("member : %s, offset : %d\n", memberName.c_str(), offset);
            }

            VkConstantBuffer* cb = new VkConstantBuffer(uniformBufferSize, uniformBufferName, binding, cbType);
            cbs.push_back(cb);

            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = binding;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            // buffer array length
            uboLayoutBinding.descriptorCount = 1;
            uboLayoutBinding.stageFlags = stageFlags;
            uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

            descriptorSetLayoutBindings.push_back(uboLayoutBinding);
        }

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
            std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings{};
            GetShaderResourcesInfo(vert_spirv, descriptorSetLayoutBindings, this, VK_SHADER_STAGE_VERTEX_BIT);
            GetShaderResourcesInfo(frag_spirv, descriptorSetLayoutBindings, this, VK_SHADER_STAGE_FRAGMENT_BIT);

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = descriptorSetLayoutBindings.size();
            layoutInfo.pBindings = descriptorSetLayoutBindings.data();

            VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout))
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

    const std::string ShaderResource::DEFAULTSHADERPATH = "triangle.shader";
    Shader ShaderResource::defaultShader = nullptr;

    ShaderResource::~ShaderResource()
    {
        vkDestroyPipeline(device, m_Pipeline, nullptr);
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
            m_Type = ShaderType::vertexFrag;
            try
            {
                m_ShaderModules = IShaderModule(new VertexFragShaderModule(path, m_Name));
            }
            catch (const std::exception& e)
            {
                m_Type = ShaderType::none;
                printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
            }
        }
        else if (ext.compare("compute") == 0)
        {
            m_Type = ShaderType::compute;
            try
            {
                m_ShaderModules = IShaderModule(new ComputeShaderModule(path, m_Name));
            }
            catch (const std::exception& e)
            {
                m_Type = ShaderType::none;
                printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
            }
        }
        else
        {
            m_Type = ShaderType::none;
        }

        // tempCode
        m_PassType = simplePass;
    }

    Shader ShaderResource::GetDefaultShader()
    {
        if (!defaultShader)
        {
            defaultShader = Shader(new ShaderResource(DEFAULTSHADERPATH));
        }
        return defaultShader;
    }

    IShaderModule ShaderResource::GetShaderModule()
    {
        if (m_Type == ShaderType::none)
        {
            auto shader = GetDefaultShader();
            if (shader->m_Type != ShaderType::none)
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
