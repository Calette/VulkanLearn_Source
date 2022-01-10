#pragma once
#include "Camera.h"
#include "IComponent.h"

namespace Palette
{
	class CameraComponent : public IComponent
	{
	public :
		CameraComponent(IObject* parent = nullptr);

	protected:
		Camera* m_Camera;
	};
}