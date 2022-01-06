#include "World.h"
#include "ModelComponent.h"

namespace Palette
{
	void World::Initialize()
	{

	}

	void World::Tick_ot()
	{
		for (auto entity : entities)
		{
			for (auto comp : entity.second->GetAllComponents())
			{
				comp->Tick_ot();
			}
		}
	}
}
