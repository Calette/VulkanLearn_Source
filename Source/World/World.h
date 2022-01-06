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

		void AddEntity(Entity* entity) { entities.emplace(entity->GetGuid(), entity); }
		void RemoveEntity(Entity* entity) { entities.erase(entity->GetGuid()); }
		std::unordered_map<char*, Entity*>& GetEntityList() { return entities; }

		void Tick_ot();

	protected:
		std::unordered_map<char*, Entity*> entities;
	};
}
