#pragma once
#include "IComponent.h"
#include "Resource/Material.h"

namespace Palette
{
	class IPrimitiveComponent : public IComponent
	{
	public:
		IPrimitiveComponent(IObject* parent = nullptr);
		~IPrimitiveComponent();

		virtual void CollectPrimitive() = 0;
		virtual void Pick_ot() {}
	};
}