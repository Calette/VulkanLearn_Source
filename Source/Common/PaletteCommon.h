#pragma once

namespace Palette
{
#define GET(type, name)														\
		public: type& Get##name() noexcept { return m_##name; }				\
		protected: type	m_##name;

#define CGETC(type, name)													\
		public: const type& Get##name() const noexcept { return m_##name; }	\
		protected: type	m_##name;

#define GETD(type, name, defualtValue)										\
		public: type& Get##name() noexcept { return m_##name; }				\
		protected: type	m_##name = defualtValue;

#define GETSET(type, name)													\
		public: void Set##name(type& value) noexcept { m_##name = value; }	\
		public: type& Get##name() noexcept { return m_##name; }				\
		protected: type	m_##name;
}
