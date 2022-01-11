#include "FrameGraph.h"
#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
	using PaletteGlobal::vulkanDevice;

	void FrameGraph::_SetFinalPass(IRenderPass* pass)
	{
		pass->SetFinalOutput();
		if (finalPass != pass)
		{
			if (finalPass)
				vulkanDevice->FreeFramebuffers();
			vulkanDevice->CreateFramebuffers(pass->GetRenderPass());
			finalPass = pass;
		}
		pass->SetExtent(vulkanDevice->GetExtent2D());
		pass->SetFramebuffer(vulkanDevice->GetSwapChainFramebuffer());
	}

	void FrameGraph::Setup_rt(IRenderPass* pass) noexcept
	{
		m_Passes.clear();

		_SetFinalPass(pass);
		pass->SetEffective();
		m_Passes.emplace_front(pass);
		auto preNodes = pass->GetInputNodes();
		while (preNodes.size() != 0)
		{
			pass = pass->GetInputNodes()[0];
			pass->SetEffective();
			m_Passes.emplace_front(pass);
			auto preNodes = pass->GetInputNodes();
		}
	}

	void FrameGraph::Execute_rt() noexcept
	{
		VkCommandBuffer cmd = PaletteGlobal::vulkanDevice->GetCommandBuffer();

		for (IRenderPass* pass : m_Passes)
		{
			if(pass->IsEffective())
				_RenderPass_rt(pass, cmd);
		}
	}

	void FrameGraph::_RenderPass_rt(IRenderPass* pass, VkCommandBuffer& cmd) noexcept
	{
		pass->Initialize(cmd);
		pass->Render_rt();
		pass->FinishRender();
	}
}