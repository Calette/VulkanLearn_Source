#include "SceneCuller.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "World/IPrimitiveComponent.h"
#include "World/ModelComponent.h"

namespace Palette
{
	void SceneCuller::Culling(Camera* camera, RenderScene* scene)
	{
		auto entityList = PaletteGlobal::client->GetWorld()->GetEntityList();

		if (entityList.size() == 0)
		{
			Entity* entity = new Entity();
			ModelComponent* model = entity->AddComponent<ModelComponent>();
			PaletteGlobal::client->GetWorld()->AddEntity(entity);
		}

		for (auto entity : entityList)
		{
			IPrimitiveComponent* primitive = entity.second->GetComponent<IPrimitiveComponent>();
			if (primitive)
				primitive->CollectPrimitive_rt(camera, scene);
		}
	}
}