#pragma once

namespace Palette
{
	class IRenderElement
	{
	public:
		virtual void _DrawItem_rt() = 0;
	};
}