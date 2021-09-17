#include <typeinfo>

#include "Entity.h"
#include "TransformComponent.h"

namespace Palette
{
	Entity::Entity()
	{
		m_Transform = new TransformComponent();
		AddComponent(m_Transform);
	}

	Entity::~Entity()
	{
		for (IComponent* component : m_Components)
		{
			delete component;
		}
		m_Components.clear();
	}

	void Entity::AddComponent(IComponent* component)
	{
		m_Components.push_back(component);
		GetComponent<TransformComponent>();
	}

}