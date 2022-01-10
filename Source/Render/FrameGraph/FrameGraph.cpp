#include "FrameGraph.h"
#include "Render/Vulkan/VulkanGlobal.h"

namespace Palette
{
	void FrameGraph::Setup_rt(IRenderPass* pass) noexcept
	{
		m_Passes.clear();

		m_Passes.emplace_front(pass);
		auto preNodes = pass->GetInputNodes();
		while (preNodes.size() != 0)
		{
			pass = pass->GetInputNodes()[0];
			m_Passes.emplace_front(pass);
			auto preNodes = pass->GetInputNodes();
		}
	}

	void FrameGraph::Execute_rt() noexcept
	{
		VkCommandBuffer cmd = PaletteGlobal::vulkanDevice->GetCommandBuffer();

		for (IRenderPass* pass : m_Passes)
		{
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