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

	void IShaderModuleResourse::GetDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets)
	{
		int set = 0;
		auto setIter = m_DescriptorSets.find(set++);
		while(setIter != m_DescriptorSets.end())
		{
			descriptorSets.push_back(setIter->second[PaletteGlobal::vulkanDevice->GetImageIndex()]);
			setIter = m_DescriptorSets.find(set++);
		}
	}

	void IShaderModuleResourse::GetDescriptorSetLayouts(std::vector<VkDescriptorSetLayout>& descriptorSetLayouts)
	{
		int set = 0;
		auto layoutIter = m_DescriptorSetLayouts.find(set++);
		while (layoutIter != m_DescriptorSetLayouts.end())
		{
			descriptorSetLayouts.push_back(layoutIter->second);
			layoutIter = m_DescriptorSetLayouts.find(set++);
		}
	}

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
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();

		for (auto& layoutIter : m_DescriptorSetLayouts)
		{
			std::vector<VkDescriptorSetLayout> layouts(size, layoutIter.second);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_DescriptorPool;
			allocInfo.descriptorSetCount = size;
			allocInfo.pSetLayouts = layouts.data();

			m_DescriptorSets[layoutIter.first].resize(size);
			VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets[layoutIter.first].data()))
		}
	}

	void IShaderModuleResourse::_InitParamters()
	{
		auto& Parameters = m_Shader->GetParameters();
		for (auto& iter : Parameters)
		{
			auto& param = iter.second;
			// tempCode
			VkConstantBuffer* buffer = m_ConstantBuffers[param.set][param.binding];
			if (buffer->GetType() == CUSTOM_CONSTANT)
			{
				CustomConstant* cb = dynamic_cast<CustomConstant*>(buffer->GetConstantBuffer());
				param.targetData = (char*)cb->GetData() + param.offset;
				// todo default value
				memset(param.targetData, 0, param.size / sizeof(int));
			}
		}
	}

	void IShaderModuleResourse::_CreateDescriptorPool()
	{
		uint32_t size = PaletteGlobal::vulkanDevice->GetImageCount();
		VkDescriptorPoolSize poolSizes[3];
		uint32_t poolSizeCount = 0;

		uint32_t count = 0;
		for (auto& uniformIter : m_ConstantBuffers)
		{
			count += uniformIter.second.size();
		}
		VkDescriptorPoolSize uniformPoolSize{};
		uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformPoolSize.descriptorCount = size * count;
		if (count > 0)
			poolSizes[poolSizeCount++] = uniformPoolSize;

		// todo
		//uint32_t count = 1;
		//VkDescriptorPoolSize samplerPoolSize{};
		//uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//uniformPoolSize.descriptorCount = size * count;
		//if (count > 0)
		//    poolSizes[poolSizeCount++] = uniformPoolSize;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizeCount;
		poolInfo.pPoolSizes = poolSizes;
		// tempCode
		poolInfo.maxSets = 512;

		VK_CHECK_RESULT(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool))
	}

	void VertexFragShaderModule::OnRefDestroy()
	{
		vkDestroyShaderModule(device, m_VertShaderModule, nullptr);
		vkDestroyShaderModule(device, m_FragShaderModule, nullptr);

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		int set = 0;
		auto layoutIter = m_DescriptorSetLayouts.find(set++);
		if (m_BufferType & GLOBAL_CONSTANT)
			layoutIter = m_DescriptorSetLayouts.find(set++);

		while (layoutIter != m_DescriptorSetLayouts.end())
		{
			vkDestroyDescriptorSetLayout(device, layoutIter->second, nullptr);
			layoutIter = m_DescriptorSetLayouts.find(set++);
		}

		for (auto& iter : m_ConstantBuffers)
		{
			for (auto buffer : iter.second)
			{
				if (buffer->GetType() == CUSTOM_CONSTANT)
					delete buffer;
			}
		}
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

	VertexFragShaderModule::VertexFragShaderModule(const std::string& path, const std::string& name, ShaderResource* shader)
	{
		m_Shader = shader;

		std::string m_Vert_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".vert";
		std::string m_Frag_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".frag";

		m_GLSL_Path = GetShaderPath() + "Shaders/GLSL/" + name + ".shader";

		// todo parse
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
		m_Frag_SPIR_V_Path = GetShaderPath() + "Shaders/SPIR-V/" + name + "_pixel.spv";

		_CreateShaderModule(m_VertShaderModule, vert_spirv);
		_CreateShaderModule(m_FragShaderModule, frag_spirv);

		_CreatePipelineShaderStage();

		_CreateShaderInfo(vert_spirv, frag_spirv);

		_CreateDescriptorPool();

		_CreateDescriptorSet();

		_SetConstantBuffer();

		_InitParamters();
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

	static void GetShaderResourcesInfo(std::vector<std::uint32_t>& sourceCode, std::unordered_map<unsigned, std::vector<VkDescriptorSetLayoutBinding>>& descriptorSetLayoutBindings, VertexFragShaderModule* shaderModule, VkShaderStageFlags stageFlags)
	{
		spirv_cross::Compiler compiler(sourceCode);
		// The SPIR-V is now parsed, and we can perform reflection on it.
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		auto& paramters = shaderModule->GetShader()->GetParameters();
		auto& cbs = shaderModule->GetConstantBuffers();
		auto& bufferType = shaderModule->GetBufferType();

		// Get all Uniform Buffer in the shader
		for (auto& resource : resources.uniform_buffers)
		{
			unsigned set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			unsigned binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

			const std::string& uniformBufferName = compiler.get_name(resource.id);

			if (uniformBufferName.compare(GlobalConstantBuffer::Instance()->GetGlobalConstant()->GetName()) == 0)
			{
				bufferType = (ConstantBufferType)(bufferType | GLOBAL_CONSTANT);
				continue;
			}

			spirv_cross::SPIRType uniformBufferType = compiler.get_type(resource.type_id);
			size_t uniformBufferSize = compiler.get_declared_struct_size(uniformBufferType);
			printf("[(set)%d][(bind)%d]uniform_buffers : %s, size : %d\n", set, binding, uniformBufferName.c_str(), uniformBufferSize);

			auto cbType = GlobalConstantBuffer::Instance()->GetConstantBufferType(uniformBufferName);
			VkConstantBuffer* cb = new VkConstantBuffer(uniformBufferSize, uniformBufferName, set, binding, cbType);
			auto cbIter = cbs.find(set);
			if (cbIter == cbs.end())
				cbs.emplace(set, std::vector<VkConstantBuffer*>{cb});
			else
				cbIter->second.push_back(cb);

			VkDescriptorSetLayoutBinding uboLayoutBinding{};
			uboLayoutBinding.binding = binding;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			// buffer array length
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = stageFlags;
			uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

			auto layoutIter = descriptorSetLayoutBindings.find(set);
			if (layoutIter == descriptorSetLayoutBindings.end())
				descriptorSetLayoutBindings.emplace(set, std::vector<VkDescriptorSetLayoutBinding>{uboLayoutBinding});
			else
				layoutIter->second.push_back(uboLayoutBinding);

			uint32_t member_count = uniformBufferType.member_types.size();
			for (uint32_t i = 0; i < member_count; i++)
			{
				auto& memberName = compiler.get_member_name(resource.base_type_id, i);
				auto& memberType = compiler.get_type(uniformBufferType.member_types[i]);
				size_t memberSize = compiler.get_declared_struct_member_size(uniformBufferType, i);
				uint32_t offset = compiler.type_struct_member_offset(uniformBufferType, i);

				paramters.emplace(memberName, ShaderParameter{
					memberName,
					SPIRTypeToShaderParameterType(memberType, memberSize),
					set,
					binding,
					offset,
					memberSize,
					malloc(memberSize),
					nullptr });
				printf("member : %s, offset : %d\n", memberName.c_str(), offset);
			}
		}

		for (auto& resource : resources.storage_buffers)
		{

		}

		// Get all sampled images in the shader.
		for (auto& resource : resources.sampled_images)
		{

		}

		for (auto& resource : resources.storage_images)
		{

		}
	}

	void VertexFragShaderModule::_CreateShaderInfo(std::vector<std::uint32_t>& vert_spirv, std::vector<std::uint32_t>& frag_spirv)
	{
		try
		{
			std::unordered_map<unsigned, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings{};
			GetShaderResourcesInfo(vert_spirv, descriptorSetLayoutBindings, this, VK_SHADER_STAGE_VERTEX_BIT);
			GetShaderResourcesInfo(frag_spirv, descriptorSetLayoutBindings, this, VK_SHADER_STAGE_FRAGMENT_BIT);

			for (auto& iter : descriptorSetLayoutBindings)
			{
				VkDescriptorSetLayoutCreateInfo layoutInfo{};
				layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				layoutInfo.bindingCount = iter.second.size();
				layoutInfo.pBindings = iter.second.data();

				VkDescriptorSetLayout layout{};
				VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout))
				m_DescriptorSetLayouts.emplace(iter.first, layout);
			}
		}
		catch (const spirv_cross::CompilerError& e)
		{
			std::cout << (e.what());
			abort();
		}
	}

	void VertexFragShaderModule::_SetConstantBuffer()
	{
		if (m_BufferType & GLOBAL_CONSTANT)
		{
			m_DescriptorSetLayouts.emplace(GlobalConstantBuffer::Instance()->GetSet(), GlobalConstantBuffer::Instance()->GetDescriptorSetLayout());
			m_DescriptorSets.emplace(GlobalConstantBuffer::Instance()->GetSet(), GlobalConstantBuffer::Instance()->GetDescriptorSets());
		}

		for (auto& iter : m_ConstantBuffers)
		{
			for (auto buffer : iter.second)
			{
				if (buffer->GetType() == CUSTOM_CONSTANT)
					buffer->CreateDescriptorSet(m_DescriptorSets[iter.first]);
			}
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

	ComputeShaderModule::ComputeShaderModule(const std::string& path, const std::string& name, ShaderResource* shader)
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
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyPipeline(device, m_Pipeline, nullptr);

		for (auto param : m_Parameters)
		{
			free(param.second.defaultData);
		}
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
				m_ShaderModule = IShaderModule(new VertexFragShaderModule(path, m_Name, this));
			}
			catch (const std::exception& e)
			{
				m_Type = ShaderType::errorShader;
				printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
			}
		}
		else if (ext.compare("compute") == 0)
		{
			m_Type = ShaderType::compute;
			try
			{
				m_ShaderModule = IShaderModule(new ComputeShaderModule(path, m_Name, this));
			}
			catch (const std::exception& e)
			{
				m_Type = ShaderType::errorShader;
				printf("can not find the shader file : s% : s%", m_Name.c_str(), e.what());
			}
		}
		else
		{
			m_Type = ShaderType::errorShader;
		}

		// tempCode
		m_PassType = SIMPLE_PASS;
	}

	Shader ShaderResource::GetDefaultShader()
	{
		if (!defaultShader)
		{
			defaultShader = Shader(new ShaderResource(DEFAULTSHADERPATH));
		}
		return defaultShader;
	}

	void ShaderResource::ReleaseDefaultShader()
	{
		defaultShader = nullptr;
	}

	IShaderModule ShaderResource::GetShaderModule()
	{
		if (m_Type == ShaderType::errorShader)
		{
			auto shader = GetDefaultShader();
			if (shader->m_Type != ShaderType::errorShader)
				return shader->GetShaderModule();
			else
				throw std::runtime_error("defaultShader fails to load");
		}
		return m_ShaderModule;
	}

	void ShaderResource::CreatePipeline(IRenderPass* pass)
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
		m_ShaderModule->GetDescriptorSetLayouts(DescriptorSetLayoutList);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = DescriptorSetLayoutList.size();
		pipelineLayoutInfo.pSetLayouts = DescriptorSetLayoutList.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout))

		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		// tempCode
		VkVertexInputBindingDescription bindingDescription;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		GetVertexInputDescriptions(VertexFormat::VERTEX_P3_C3_U2, bindingDescription, attributeDescriptions);

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
		pipelineInfo.pStages = m_ShaderModule->GetShaderStages();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		//pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		//pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = pass->GetRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		//pipelineInfo.basePipelineIndex = -1; // Optional

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline))
	}

	void ShaderResource::PrepareRendering(IRenderPass* pass)
	{
		if (!m_HasPipeline)
		{
			CreatePipeline(pass);
			m_HasPipeline = true;
		}
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
