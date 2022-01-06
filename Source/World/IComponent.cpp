#include "IComponent.h"
#include "Entity.h"

namespace Palette
{
	IComponent::IComponent(IObject* parent)
		: m_Parent(parent)
	{

	}

	const TransformComponent* IComponent::Transform()
	{ 
		return static_cast<Entity*>(m_Parent)->Transform(); 
	}
}
