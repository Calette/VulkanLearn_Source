#pragma once
#include "Render/IRenderPass.h"

namespace Palette
{
	class SimplePass : public IRenderPass
	{
	public:
		SimplePass() { m_Name = "Simple Pass"; m_PassType = PassType::simplePass; }
		~SimplePass() {}

		IRenderPass* Connect(IRenderPass* pass, RenderScene* scene);
	};
}