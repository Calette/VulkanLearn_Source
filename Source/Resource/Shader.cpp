#include <fstream>

#include "Shader.h"
#include "../Common/FileSystem.h"
#include "../Render/Vulkan/VulkanGlobal.h"
#include "fmt/printf.h"
#include "../Render/VertexInputBindingDescription.h"

namespace Palette
{
    IShaderModule::IShaderModule(const std::string& path) 
    {

    }

    void IShaderModule::_CreateShaderModule(VkShaderModule& shaderModule, const std::vector<char>& code)
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
        : IShaderModule(path)
    {
        //...×ª³ÉSpir_v

        //m_Vert_SPIR_V_Path = shaderPath + "SPIR-V/" + name + "_vert.spv";
        //m_Pixel_SPIR_V_Path = shaderPath + "SPIR-V/" + name + "_pixel.spv";
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
        : IShaderModule(path)
    {
        
    }

    void ComputeShaderModule::OnRefDestroy()
    {

    }

    void ComputeShaderModule::_CreatePipelineShaderStage(VkShaderModule* computeShaderModule)
    {
        
    }

    const std::string Shader::defualtShaderPath = "triangle.shader";
    Shader* Shader::defaultShader = nullptr;

    Shader::~Shader()
    {
        //for (auto shaderModules : m_ShaderModules)
        //{
        //    shaderModules.second;
        //}
        m_ShaderModules.clear();
    }

    Shader::Shader(const std::string& path)
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
                IShaderModule* shaderModule = new VertexPixelShaderModule(path, m_Name);
                m_ShaderModules.emplace(0u, shaderModule);
            }
            catch (const std::exception&)
            {
                m_Type = ShaderType::None;
            }
        }
        else if (ext.compare("compute") == 0)
        {
            m_Type = ShaderType::Compute;
            try
            {
                IShaderModule* shaderModule = new ComputeShaderModule(path, m_Name);
                m_ShaderModules.emplace(0u, shaderModule);
            }
            catch (const std::exception&)
            {
                m_Type = ShaderType::None;
            }
        }
        else
        {
            m_Type = ShaderType::None;
        }
    }

    Shader* Shader::GetDefaultShader()
    {
        if (defaultShader == nullptr)
        {
            defaultShader = new Shader(defualtShaderPath);
        }
        return defaultShader;
    }

    TSharedPtr<IShaderModule> Shader::GetShaderModule(uint32_t defineHash)
    {
        if (m_Type == ShaderType::None)
            return GetDefaultShader()->GetShaderModule(0);
        return m_ShaderModules[defineHash];
    }

    void Shader::RealeaseShaderModule(uint32_t defineHash)
    {
        m_ShaderModules.erase(m_ShaderModules.find(defineHash));
    }

    void Shader::RealeaseAllShaderModule()
    {
        m_ShaderModules.clear();
    }

    void Shader::_ReloadShader(uint32_t newTimeStamp)
    {
        if (newTimeStamp == m_TimeStamp)
            return;
        m_TimeStamp = newTimeStamp;

        //...
    }

}
