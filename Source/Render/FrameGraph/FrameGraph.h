#pragma once
#include <deque>
#include "Render/IRenderPass.h"

namespace Palette
{
	class FrameGraphNode
	{

	};

	class FrameGraphBuilder
	{

	};

	class FrameGraphExecutive
	{


	};

	class FrameGraph
	{
	public:
		void Setup_rt(IRenderPass* pass) noexcept;

		void Execute_rt() noexcept;

	protected:
		void _RenderPass_rt(IRenderPass* pass, VkCommandBuffer& cmd) noexcept;
		void _SetFinalPass(IRenderPass* pass);

	protected:
		std::deque<IRenderPass*> m_Passes;
		IRenderPass* finalPass = nullptr;
	};
}