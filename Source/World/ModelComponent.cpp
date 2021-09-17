#include "ModelComponent.h"

namespace Palette
{
	ModelComponent::ModelComponent()
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
}
