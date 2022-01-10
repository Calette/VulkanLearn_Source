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

	void CreateRenderPass(VkRenderPass& renderPass, const PassInfo& info);
	void SetFrameBuffer(IRenderPass* renderPass);
	void BeginRenderPass(IRenderPass* renderPass);
	void CreatePipeline(IRenderPass* pass, Shader shader);

	class IRenderPass
	{
	public:
		IRenderPass();
		~IRenderPass();

		virtual void Initialize(VkCommandBuffer& currCmd);
		virtual void Update_rt();
		virtual void Render_rt();
		virtual void FinishRender();

		bool IsEffective() const noexcept { return m_Effective; }
		void SetEffective() noexcept { m_Effective = true; }
		bool IsFinalOutput() const noexcept { return m_FinalOutput; }
		void SetFinalOutput() noexcept { m_FinalOutput = true; }

		const std::vector<IRenderPass*>& GetInputNodes() const noexcept { return m_InputNodes; }

		void AddIfInputNode(IRenderPass* pass) noexcept { if (pass) { m_InputNodes.emplace_back(pass); } }

		std::string& GetName() noexcept { return m_Name; }
		PassType GetPassType() noexcept { return m_Type; }
		void SetFramebuffer(VkFramebuffer& framebuffer) noexcept { m_FrameBuffer = framebuffer; }
		VkFramebuffer& GetFramebuffer() noexcept { return m_FrameBuffer; }
		void SetExtent(VkExtent2D& extent) noexcept { m_Extent = extent; }
		VkExtent2D& GetExtent() noexcept { return m_Extent; }
		VkRenderPassBeginInfo& GetRenderPassBeginInfo() noexcept { return m_RenderPassBeginInfo; }
		VkDescriptorSetLayout* GetDescriptorSetLayout() noexcept { return m_DescriptorSetLayout; }
		VkPipelineLayout& GetPipelineLayout() noexcept { return m_PipelineLayout; }
		VkRenderPass& GetRenderPass() noexcept { return m_RenderPass; }
		VkCommandBuffer& GetCommandBuffer() noexcept { return m_Cmd; }

	protected:
		std::vector<RenderElement*> m_Elements;
		std::vector<IRenderPass*> m_InputNodes;

		bool m_Effective = false;
		bool m_FinalOutput = false;

		RenderScene* m_Scene = nullptr;

		PassInfo m_info;

		VkRenderPass m_RenderPass;
		VkRenderPassBeginInfo m_RenderPassBeginInfo;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout* m_DescriptorSetLayout;
		VkFramebuffer m_FrameBuffer;
		VkExtent2D m_Extent;
		VkCommandBuffer m_Cmd;

		std::string m_Name = "Default Pass";
		PassType m_Type = PassType::DefualtPass;
	};
}
