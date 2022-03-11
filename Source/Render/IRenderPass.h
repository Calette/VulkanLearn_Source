#pragma once
#include <vector>
#include "RenderElement.h"
#include "RenderScene.h"
#include "Common/PaletteCommon.h"

namespace Palette
{
	class IRenderPass;

	struct PassInfo
	{
		VkFormat format;
	};

	//void CreateDescriptorSetLayout(VkDescriptorSetLayout& descriptorSetLayout);
	void CreateRenderPass(VkRenderPass& renderPass, const PassInfo& info);
	void CreatePipeline(IRenderPass* pass, Shader shader);
	void InitFrameBuffer(IRenderPass* renderPass);
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

		bool IsEffective() const noexcept { return m_Effective; }
		void SetEffective() noexcept { m_Effective = true; }
		bool IsFinalOutput() const noexcept { return m_FinalOutput; }
		void SetFinalOutput() noexcept { m_FinalOutput = true; }

		GETD(std::string, Name, "Default Pass")
		GETD(PassType, PassType, DEFAULT_PASS)
		GETD(ConstantBufferType, ConstantBufferType, CUSTOM_CONSTANT)

		GET(std::vector<IRenderPass*>, InputNodes)

		GET(VkCommandBuffer, CommandBuffer)
		GETSET(VkFramebuffer, Framebuffer)
		GETSET(VkExtent2D, Extent)

		GET(VkRenderPass, RenderPass)
		GET(VkRenderPassBeginInfo, RenderPassBeginInfo)

	protected:
		bool							m_Effective		= false;
		bool							m_FinalOutput	= false;

		std::vector<RenderElement*>		m_Elements;

		RenderScene*					m_Scene			= nullptr;
		PassInfo						m_info;
	};
}
