#pragma once
#include "IPrimitiveComponent.h"
#include "ModelRenderer.h"

namespace Palette
{
	class ModelComponent : public IPrimitiveComponent
	{
	public:
		ModelComponent(IObject* parent = nullptr);
		~ModelComponent();

		ModelRenderer* GetModelRenderer() { return m_ModelRenderer; }
		void Tick_ot();
		void Pick_ot() override;
		void CollectPrimitive() override;

	protected:
		ModelRenderer* m_ModelRenderer;
	};
}
