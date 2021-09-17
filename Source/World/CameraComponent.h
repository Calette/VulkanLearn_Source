#pragma once
#include "Camera.h"
#include "IComponent.h"

namespace Palette
{
	class CameraComponent : public IComponent
	{
	protected:
		Camera* camera;
	};
}