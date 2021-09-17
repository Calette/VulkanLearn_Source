#pragma once
#include <unordered_map>

#include "Entity.h"

namespace Palette
{
	class World : public IObject
	{
	public:
		World() {};
		~World() 
		{
			for (auto entityIt : entities)
			{
				delete entityIt.second;
			}
		};

		void Initialize();

		void AddEntity(Entity* entity) { entities.emplace(entity->GetObjectID(), entity); }
		void RemoveEntity(Entity* entity) { entities.erase(entity->GetObjectID()); }
		std::unordered_map<uint32_t, Entity*>& GetEntityList() { return entities; }

	protected:
		std::unordered_map<uint32_t, Entity*> entities;
	};
}
