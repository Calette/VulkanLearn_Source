#include "IRenderPass.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "Render/Shader/ConstantBuffer.h"
#include "Render/Vulkan/VulkanCommon.h"

namespace Palette
{
    using PaletteGlobal::device;
    
    void CreateRenderPass(VkRenderPass& renderPass, const PassInfo& info)
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = info.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        //VkSubpassDependency dependency{};
        //dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //dependency.dstSubpass = 0;
        //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //dependency.srcAccessMask = 0;
        //dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        // todo 据说有问题，subpass只有一个的时候建议先空着
        //renderPassInfo.dependencyCount = 1;
        //renderPassInfo.pDependencies = &dependency;

        VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass))
    }

    void CreatePipeline(IRenderPass* pass, Shader shader)
    {
        VkExtent2D& extent = pass->GetExtent();

        // Viewports and scissors
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = extent.width;
        viewport.height = extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        // beyond the near and far planes are clamped to them as opposed to discarding them. 
        // This is useful in some special cases like shadow maps.
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDescriptorSetLayout> DescriptorSetLayoutList{};
        shader->GetShaderModule()->GetDescriptorSetLayouts(DescriptorSetLayoutList);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = DescriptorSetLayoutList.size();
        pipelineLayoutInfo.pSetLayouts = DescriptorSetLayoutList.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &shader->GetPipelineLayout()))

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        // tempCode
        VkVertexInputBindingDescription bindingDescription;
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        GetVertexInputDescriptions(VertexFormat::VERTEX_P2_C3_U2, bindingDescription, attributeDescriptions);

        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shader->GetShaderModule()->GetShaderStages();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        //pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        //pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = shader->GetPipelineLayout();
        pipelineInfo.renderPass = pass->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        //pipelineInfo.basePipelineIndex = -1; // Optional

        VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shader->GetPipeline()))
    }

    void InitFrameBuffer(IRenderPass* renderPass)
    {
        //VkFramebuffer framebuffer = renderPass->GetFramebuffer();
        //VkExtent2D extent = renderPass->GetExtent();
        //if (renderPass->IsFinalOutput())
        //{
        //    framebuffer = PaletteGlobal::vulkanDevice->GetSwapChainFramebuffer();
        //    extent = PaletteGlobal::vulkanDevice->GetExtent2D();
        //}
        //else
        //{

        //}
    }

    void BeginRenderPass(IRenderPass* renderPass)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass->GetRenderPass();
        renderPassInfo.framebuffer = renderPass->GetFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = renderPass->GetExtent();

        VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(renderPass->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    IRenderPass::IRenderPass()
        : m_info()
        , m_CommandBuffer(VK_NULL_HANDLE)
        , m_Framebuffer(VK_NULL_HANDLE)
    {
        m_info.format = VK_FORMAT_B8G8R8A8_SRGB;

        CreateRenderPass(m_RenderPass, m_info);
    }

    IRenderPass::~IRenderPass()
    {
        vkDestroyRenderPass(device, m_RenderPass, nullptr);
    }

    void IRenderPass::Update_rt()
    {
        for (auto element : m_Elements)
        {
            delete element;
        }
        m_Elements.clear();
        m_Effective = false;
        m_FinalOutput = false;
    }

    void IRenderPass::Initialize(VkCommandBuffer& cmd)
    {
        m_CommandBuffer = cmd;
        InitFrameBuffer(this);
        BeginRenderPass(this);
    }

    void IRenderPass::Render_rt()
    {
        for (auto element : m_Scene->GetElements())
        {
            if(PrepareElement_rt(element, this))
                DrawElement_rt(m_CommandBuffer, this, element);
        }

        for (auto element : m_Elements)
        {
            if (PrepareElement_rt(element, this))
                DrawElement_rt(m_CommandBuffer, this, element);
        }
    }

    void IRenderPass::FinishRender()
    {
        vkCmdEndRenderPass(m_CommandBuffer);
    }
}