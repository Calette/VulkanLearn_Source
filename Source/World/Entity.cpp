#include <typeinfo>

#include "Entity.h"
#include "TransformComponent.h"

namespace Palette
{
	Entity::Entity()
	{
		m_Transform = AddComponent<TransformComponent>();
	}

	Entity::~Entity()
	{
		for (IComponent* component : m_Components)
		{
			delete component;
		}
		m_Components.clear();
	}
}