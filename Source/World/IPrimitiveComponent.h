#pragma once
#include "IComponent.h"
#include "Resource/Material.h"
#include "Camera.h"
#include "Render/RenderScene.h"

namespace Palette
{
	class IPrimitiveComponent : public IComponent
	{
	public:
		IPrimitiveComponent(IObject* parent = nullptr);
		~IPrimitiveComponent();

		virtual void CollectPrimitive_rt(Camera* camera, RenderScene* scene) = 0;
		virtual void Pick_ot() {}
	};
}