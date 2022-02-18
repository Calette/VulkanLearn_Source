#pragma once
#include "IComponent.h"
#include "Common/Math/Vector.h"

namespace Palette
{
	class TransformComponent : public IComponent
	{
	public:
		TransformComponent(IObject* parent = nullptr);
		//void Update_ot() override;

	protected:
		Vector3 position;
	};
}
