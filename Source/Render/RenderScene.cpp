#include "RenderScene.h"

namespace Palette
{
	RenderScene::RenderScene(Camera* cam)
	{
		m_Camera = cam;
	}

	RenderScene::~RenderScene()
	{
		Reset();
	}

	void RenderScene::AddElement_rt(RenderElement* element)
	{
		m_Elements.push_back(element);
	}

	void RenderScene::Reset()
	{
		for (auto element : m_Elements)
		{
			delete element;
		}
		m_Elements.clear();
	}
}