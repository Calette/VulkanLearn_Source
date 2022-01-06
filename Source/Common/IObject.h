#pragma once
#include <iostream>
#include <string>
#include "Guid.h"

namespace Palette
{
	class IObject
	{
	public:
		IObject()
		{
			uuid4_generate(m_Guid);
		}
		virtual ~IObject() {};

		std::string GetName() const noexcept { return m_Name; }

		char* GetGuid() noexcept { return m_Guid; }

	protected:
		std::string m_Name;

		char m_Guid[UUID_LEN];
	};
}
