#pragma once
#include "IPrimitiveComponent.h"
#include "ModelRenderer.h"

namespace Palette
{
	class ModelComponent : public IPrimitiveComponent
	{
	public:
		ModelComponent();
		~ModelComponent();

		ModelRenderer* GetModelRenderer() { return m_ModelRenderer; }
		void Tick_ot();

	protected:
		ModelRenderer* m_ModelRenderer;
	};
}
