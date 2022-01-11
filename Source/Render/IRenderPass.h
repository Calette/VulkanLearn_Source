#pragma once
#include <vector>
#include "RenderElement.h"
#include "RenderScene.h"

namespace Palette
{
	class IRenderPass;

	struct PassInfo
	{
		VkFormat format;
	};

	void CreateDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout);
	void CreateRenderPass(VkRenderPass& renderPass, const PassInfo& info);
	void CreatePipeline(IRenderPass* pass, Shader shader);
	void SetFrameBuffer(IRenderPass* renderPass);
	void BeginRenderPass(IRenderPass* renderPass);

	class IRenderPass
	{
	public:
		IRenderPass();
		~IRenderPass();

		virtual void Update_rt();
		virtual void Initialize(VkCommandBuffer& currCmd);
		virtual void Render_rt();
		virtual void FinishRender();

		void AddIfInputNode(IRenderPass* pass) noexcept { if (pass) { m_InputNodes.emplace_back(pass); } }

		const std::vector<IRenderPass*>& GetInputNodes() const noexcept { return m_InputNodes; }

		bool IsEffective() const noexcept { return m_Effective; }
		void SetEffective() noexcept { m_Effective = true; }
		bool IsFinalOutput() const noexcept { return m_FinalOutput; }
		void SetFinalOutput() noexcept { m_FinalOutput = true; }

		std::string& GetName() noexcept { return m_Name; }
		PassType GetPassType() noexcept { return m_Type; }
		VkCommandBuffer& GetCommandBuffer() noexcept { return m_Cmd; }
		void SetFramebuffer(VkFramebuffer& framebuffer) noexcept { m_FrameBuffer = framebuffer; }
		VkFramebuffer& GetFramebuffer() noexcept { return m_FrameBuffer; }
		void SetExtent(VkExtent2D& extent) noexcept { m_Extent = extent; }
		VkExtent2D& GetExtent() noexcept { return m_Extent; }
		VkRenderPass& GetRenderPass() noexcept { return m_RenderPass; }
		VkRenderPassBeginInfo& GetRenderPassBeginInfo() noexcept { return m_RenderPassBeginInfo; }
		VkPipelineLayout& GetPipelineLayout() noexcept { return m_PipelineLayout; }
		VkDescriptorSet* GetDescriptorSet() noexcept { return m_DescriptorSet; }

	protected:
		std::string					m_Name			= "Default Pass";
		PassType					m_Type			= PassType::DefualtPass;

		bool						m_Effective		= false;
		bool						m_FinalOutput	= false;

		std::vector<RenderElement*>	m_Elements;
		std::vector<IRenderPass*>	m_InputNodes;

		RenderScene*				m_Scene			= nullptr;
		PassInfo					m_info;

		VkCommandBuffer				m_Cmd;
		VkFramebuffer				m_FrameBuffer;
		VkExtent2D					m_Extent;

		VkRenderPass				m_RenderPass;
		VkRenderPassBeginInfo		m_RenderPassBeginInfo;
		VkPipelineLayout			m_PipelineLayout;
		VkDescriptorSet*			m_DescriptorSet;
	};
}
