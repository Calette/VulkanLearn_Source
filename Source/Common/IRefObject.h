#pragma once
#include <atomic>

namespace Palette
{
	class IRefObject
	{
	public:
		void AddRefrenceCount() { m_RefrenceCount.fetch_add(1u, std::memory_order_acq_rel); }
		virtual uint32_t Release() = 0;
	protected:
		IRefObject() noexcept = default;
	protected:
		std::atomic<uint32_t> m_RefrenceCount;
	};

	class ISharedObject : public IRefObject
	{

	};
}
