#pragma once
#include "Common/IObject.h"

namespace Palette
{
	class IComponent : public IObject
	{
	public:
		IComponent();
		virtual ~IComponent() {}

		virtual void Tick_ot() = 0;
	};
}
