#include "World.h"
#include "ModelComponent.h"

namespace Palette
{
	void World::Initialize()
	{

	}

	void World::Update_ot()
	{
		for (auto entity : entities)
		{
			for (auto comp : entity.second->GetAllComponents())
			{
				comp->Update_ot();
			}
		}
	}
}
