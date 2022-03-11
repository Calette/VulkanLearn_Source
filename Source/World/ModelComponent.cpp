#include "ModelComponent.h"
#include "Render/RenderScene.h"
#include <typeinfo>

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
		for (auto mat : m_ModelRenderer->GetAllMaterials())
		{
			glm::mat4 model = glm::rotate(glm::mat4(1.0f), Time::Instance()->GetTime() * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			mat->SetMatrix4("model", model);
		}
	}

	void ModelComponent::Pick_ot()
	{

	}

	void ModelComponent::CollectPrimitive_rt(Camera* camera, RenderScene* scene)
	{
		m_ModelRenderer->AddPrimitive_rt(scene);
	}
}
