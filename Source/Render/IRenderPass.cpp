#include "IRenderPass.h"

namespace Palette
{
	void IRenderPass::_Render_rt()
	{
		for (IRenderElement* elem : m_Elements)
		{
			elem->_DrawItem_rt();
		}
	}
}