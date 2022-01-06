#pragma once
#include <vector>
#include "IRenderElement.h"

namespace Palette
{
	class FrameGraph;
	class IRenderPass
	{
	public:
		virtual void _Render_rt();

	protected:
		std::vector<IRenderElement*> m_Elements;
	};
}
