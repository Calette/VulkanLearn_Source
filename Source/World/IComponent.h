#pragma once
#include "Common/IObject.h"
#include <vector>

namespace Palette
{
	class TransformComponent;

	class IComponent : public IObject
	{
	public:
		IComponent(IObject* parent = nullptr);
		virtual ~IComponent() {}

		virtual void Update_ot() {};

		IObject* GetParent() const noexcept { return m_Parent; }

		const TransformComponent* Transform();

	protected:
		IObject* m_Parent;
	};
}
