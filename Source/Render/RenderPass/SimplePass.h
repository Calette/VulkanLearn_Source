#pragma once
#include "Render/IRenderPass.h"

namespace Palette
{
	class SimplePass : public IRenderPass
	{
	public:
		SimplePass() { m_Type = PassType::SimplePass; }
		~SimplePass() {}

		IRenderPass* Connect(IRenderPass* pass, RenderScene* scene);

		//void Render_rt() override;
	};
}