#pragma once
#include <string>

namespace Palette
{
	class IObject
	{
	public:
		IObject() : m_ObjectID(0u) {};

		std::string GetName() const noexcept { return m_Name; }

		uint32_t GetObjectID() const noexcept { return m_ObjectID; }

	protected:
		std::string m_Name;

		uint32_t m_ObjectID;
	};
}
