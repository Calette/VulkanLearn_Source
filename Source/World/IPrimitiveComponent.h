#pragma once
#include "IComponent.h"
#include "Resource/Material.h"

namespace Palette
{
	class IPrimitiveComponent : public IComponent
	{
	public:
		IPrimitiveComponent();
		~IPrimitiveComponent();

		void CollectPrimitive();
	};
}