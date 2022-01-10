#pragma once
#include <vector>
#include "World/Camera.h"
#include "RenderElement.h"

namespace Palette
{
	class RenderScene
	{
	public:
		RenderScene(Camera* camera);
		~RenderScene();

		void AddElement_rt(RenderElement* element);
		void Reset();

		Camera* GetCamera() const noexcept { return m_Camera; }
		void SetCamera(Camera* newCamera) noexcept { m_Camera = newCamera; }
		std::vector<RenderElement*> GetElements() const noexcept { return m_Elements; }
	private:
		Camera* m_Camera;
		std::vector<RenderElement*> m_Elements;
	};
}