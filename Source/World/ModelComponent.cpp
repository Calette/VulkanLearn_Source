#include "ModelComponent.h"
#include "Render/RenderScene.h"

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

	void ModelComponent::Update_ot()
	{

	}

	void ModelComponent::Pick_ot()
	{

	}

	void ModelComponent::CollectPrimitive_rt(Camera* camera, RenderScene* scene)
	{
		m_ModelRenderer->AddPrimitive_rt(scene);
	}
}
