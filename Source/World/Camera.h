#pragma once
#include "Common/IObject.h"
#include "Common/Math/Vecter.h"

namespace Palette
{
	struct SaveCamPos
	{
		Vecter3 position;

	};

	class Camera : public IObject
	{
	public:
		float GetFov() { return m_Fov; }
		float SetFov(float fov) { m_Fov = fov; }

	public:
		static Camera* MainCamera;

	protected:
		float m_Fov;

		SaveCamPos saveCamPos[6];
	};
}
