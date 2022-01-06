#include "ModelComponent.h"

namespace Palette
{
	ModelComponent::ModelComponent(IObject* parent)
		: IPrimitiveComponent(parent)
	{
		m_ModelRenderer = new ModelRenderer();
	}

	ModelComponent::~ModelComponent()
	{
		delete m_ModelRenderer;
	}

	void ModelComponent::Tick_ot()
	{

	}

	void ModelComponent::Pick_ot()
	{

	}

	void ModelComponent::CollectPrimitive()
	{

	}
}
