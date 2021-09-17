#pragma once

namespace Palette
{
	class IRenderElement
	{
		virtual void _DrawItem_on_rt() = 0;
	};
}