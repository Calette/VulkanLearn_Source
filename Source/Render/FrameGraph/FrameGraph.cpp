#include "FrameGraph.h"

namespace Palette
{
	void FrameGraph::_Execute_rt() noexcept
	{
		for (IRenderPass* pass : m_Passes)
		{
			_RenderPass_rt(pass);
		}
	}

	void FrameGraph::_RenderPass_rt(IRenderPass* pass) noexcept
	{
		pass->_Render_rt();
	}


}