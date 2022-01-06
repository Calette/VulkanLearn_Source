#pragma once

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

	protected:
		void _Execute_rt() noexcept;

		void _RenderPass_rt(IRenderPass* pass) noexcept;

	protected:
		std::vector<IRenderPass*> m_Passes;
	};
}