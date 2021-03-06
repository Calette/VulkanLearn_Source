#pragma once
#include <vector>

#include "Common/IObject.h"
#include "IComponent.h"
#include "World/TransformComponent.h"

namespace Palette
{
	class Entity : public IObject
	{
	public:
		Entity();
		~Entity();

		std::vector<IComponent*>& GetAllComponents() { return m_Components; }

		template <typename T> T* AddComponent()
		{
			T* component = new T(this);
			m_Components.push_back(component);
			return component;
		}

		template <typename T> T* GetComponent()
		{
			for (auto comp : m_Components)
			{
				T* component = dynamic_cast<T*>(comp);
				if (component)
				{
					return component;
				}
			}

			return nullptr;
		}

		template <typename T> std::vector<T*> GetComponents()
		{
			std::vector<T*> components;
			for (IComponent* comp : m_Components)
			{
				T* component = dynamic_cast<T*>(comp);
				if (component)
				{
					components.push_back(dynamic_cast<T*>(component));
				}
			}

			return components;
		}

		const TransformComponent* Transform() { return m_Transform; }

	protected:
		std::vector<IComponent*> m_Components;
		TransformComponent* m_Transform;
	};
}
