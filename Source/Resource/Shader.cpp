#include <fstream>

#include "Shader.h"
#include "../Common/FileSystem.h"
#include "../Common/MD5.h"
#include "../Render/Vulkan/VulkanGlobal.h"
#include "fmt/printf.h"
#include "../Render/Mesh/VertexInputBindingDescription.h"

namespace Palette
{
    IShaderModuleResourse::IShaderModuleResourse(const std::string& path) 
    {

    }

    void IShaderModuleResourse::_CreateShaderModule(VkShaderModule& shaderModule, const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(PaletteGlobal::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error(m_Name + " : failed to create shader module!");
        }
    }

    VertexPixelShaderModule::~VertexPixelShaderModule()
    {
        vkDestroyShaderModule(PaletteGlobal::device, m_VertShaderModule, nullptr);
        vkDestroyShaderModule(PaletteGlobal::device, m_PixelShaderModule, nullptr);
    }

    void VertexPixelShaderModule::OnRefDestroy()
    {
        vkDestroyShaderModule(PaletteGlobal::device, m_VertShaderModule, nullptr);
        vkDestroyShaderModule(PaletteGlobal::device, m_PixelShaderModule, nullptr);
    }

    VertexPixelShaderModule::VertexPixelShaderModule(const std::string& path, const std::string& name)
        : IShaderModuleResourse(path)
    {
        //...×ª³ÉSpir_v

        //m_Vert_SPIR_V_Path = SHADERPATH + "SPIR-V/" + name + "_vert.spv";
        //m_Pixel_SPIR_V_Path = SHADERPATH + "SPIR-V/" + name + "_pixel.spv";
        m_Vert_SPIR_V_Path = "Shaders/SPIR-V/" + name + "_vert.spv";
        m_Pixel_SPIR_V_Path = "Shaders/SPIR-V/" + name + "_pixel.spv";

        _CreateShaderModule(m_VertShaderModule, ReadFile(m_Vert_SPIR_V_Path));
        _CreateShaderModule(m_PixelShaderModule, ReadFile(m_Pixel_SPIR_V_Path));

        _CreatePipelineShaderStage();
    }

    void VertexPixelShaderModule::_CreatePipelineShaderStage()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = m_VertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = m_PixelShaderModule;
        fragShaderStageInfo.pName = "main";

        m_ShaderStages[0] = vertShaderStageInfo;
        m_ShaderStages[1] = fragShaderStageInfo;
    }

    ComputeShaderModule::~ComputeShaderModule()
    {

    }

    ComputeShaderModule::ComputeShaderModule(const std::string& path, const std::string& name)
        : IShaderModuleResourse(path)
    {
        
    }

    void ComputeShaderModule::OnRefDestroy()
    {

    }

    void ComputeShaderModule::_CreatePipelineShaderStage(VkShaderModule* computeShaderModule)
    {
        
    }

    const std::string ShaderResource::DEFUALTSHADERPATH = "triangle.shader";
    Shader ShaderResource::defaultShader = nullptr;

    ShaderResource::~ShaderResource()
    {
        m_ShaderModules.clear();
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
            m_Type = ShaderType::VertexPixel;
            try
            {
                IShaderModule shaderModule = IShaderModule(new VertexPixelShaderModule(path, m_Name));
                m_ShaderModules.emplace(0u, shaderModule);
            }
            catch (const std::exception&)
            {
                m_Type = ShaderType::None;
                printf("can not find the shader file : s% : ", m_Name.c_str());
            }
        }
        else if (ext.compare("compute") == 0)
        {
            m_Type = ShaderType::Compute;
            try
            {
                IShaderModule shaderModule = IShaderModule(new ComputeShaderModule(path, m_Name));
                m_ShaderModules.emplace(0u, shaderModule);
            }
            catch (const std::exception&)
            {
                m_Type = ShaderType::None;
                printf("can not find the shader file : s% : ", m_Name.c_str());
            }
        }
        else
        {
            m_Type = ShaderType::None;
        }
    }

    Shader ShaderResource::GetDefaultShader()
    {
        if (!defaultShader)
        {
            defaultShader = Shader(new ShaderResource(DEFUALTSHADERPATH));
        }
        return defaultShader;
    }

    IShaderModule ShaderResource::GetShaderModule(uint32_t defineHash)
    {
        // if defaultShader fails to load, here will be a infinite loop
        if (m_Type == ShaderType::None)
            return GetDefaultShader()->GetShaderModule(0);
        return m_ShaderModules[defineHash];
    }

    void ShaderResource::RealeaseShaderModule(uint32_t defineHash)
    {
        m_ShaderModules.erase(m_ShaderModules.find(defineHash));
    }

    void ShaderResource::RealeaseAllShaderModule()
    {
        m_ShaderModules.clear();
    }

    void ShaderResource::_ReloadShader(uint32_t newTimeStamp)
    {
        if (newTimeStamp == m_TimeStamp)
            return;
        m_TimeStamp = newTimeStamp;

        //...
    }

}
