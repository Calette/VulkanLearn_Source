#include "SimplePass.h"

namespace Palette
{
	IRenderPass* SimplePass::Connect(IRenderPass* pass, RenderScene* scene)
	{
		m_Scene = scene;
		AddIfInputNode(pass);
		return this;
	}

	//void SimplePass::Render_rt()
	//{

	//}
}