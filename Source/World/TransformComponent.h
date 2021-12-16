#pragma once
#include "IComponent.h"
#include "Common/Math/Vecter.h"

namespace Palette
{
	class TransformComponent : public IComponent
	{
	public:
		TransformComponent();
		//void Tick_ot();

	protected:
		Vecter3 position;
	};
}
