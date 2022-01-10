#pragma once
#include "Common/Singleton.h"
#include "World/Camera.h"
#include "Render/RenderScene.h"

namespace Palette
{
	class SceneCuller : public Singleton<SceneCuller>
	{
	public:
		void Culling(Camera* camera, RenderScene* renderScene);
	};
}