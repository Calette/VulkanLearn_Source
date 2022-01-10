#include "IRenderPass.h"
#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
    void CreateDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(PaletteGlobal::device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

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

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(PaletteGlobal::device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void SetFrameBuffer(IRenderPass* renderPass)
    {
        VkFramebuffer framebuffer = renderPass->GetFramebuffer();
        VkExtent2D extent = renderPass->GetExtent();
        if (renderPass->IsFinalOutput())
        {
            //framebuffer = PaletteGlobal::vulkanDevice->GetSwapChainFramebuffer();
            //extent = PaletteGlobal::vulkanDevice->GetExtent2D();
        }
        else
        {

        }
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

    void CreatePipeline(IRenderPass* pass, Shader shader)
    {
        VkExtent2D extent = pass->GetExtent();

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
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = pass->GetDescriptorSetLayout();
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(PaletteGlobal::device, &pipelineLayoutInfo, nullptr, &pass->GetPipelineLayout()) != VK_SUCCESS)
        {
            throw std::runtime_error(pass->GetName() + "failed to create pipeline layout!");
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        VkVertexInputBindingDescription bindingDescription;
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions;
        GetVertexInputDescriptions(VertexFormat::VERTEX_P2_C3, bindingDescription, attributeDescriptions);

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
        pipelineInfo.pStages = shader->GetShaderModule(0)->GetShaderStages();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        //pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        //pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = pass->GetPipelineLayout();
        pipelineInfo.renderPass = pass->GetRenderPass();
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        //pipelineInfo.basePipelineIndex = -1; // Optional

        if (vkCreateGraphicsPipelines(PaletteGlobal::device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shader->GetPipeline()) != VK_SUCCESS)
        {
            throw std::runtime_error(pass->GetName() + "failed to create graphics pipeline!");
        }
    }

    IRenderPass::IRenderPass()
        : m_info()
        , m_Cmd(VK_NULL_HANDLE)
        , m_FrameBuffer(VK_NULL_HANDLE)
    {
        m_DescriptorSetLayout = new VkDescriptorSetLayout();
        m_info.format = VK_FORMAT_B8G8R8A8_SRGB;

        CreateRenderPass(m_RenderPass, m_info);
        CreateDescriptorSetLayout(*m_DescriptorSetLayout);
    }

    IRenderPass::~IRenderPass()
    {
        vkDestroyRenderPass(PaletteGlobal::device, m_RenderPass, nullptr);
    }

    void IRenderPass::Initialize(VkCommandBuffer& cmd)
    {
        m_Cmd = cmd;
        SetFrameBuffer(this);
        BeginRenderPass(this);
    }

	void IRenderPass::Update_rt()
	{
		for (auto element : m_Elements)
		{
			delete element;
		}
		m_Elements.clear();

        //m_Effective = false;
        m_FinalOutput = false;
	}

    void IRenderPass::Render_rt()
    {
        for (auto element : m_Scene->GetElements())
        {
            DrawElement(m_Cmd, this, element);
        }

        for (auto element : m_Elements)
        {
            DrawElement(m_Cmd, this, element);
        }
    }

    void IRenderPass::FinishRender()
    {
        vkCmdEndRenderPass(m_Cmd);
    }
}