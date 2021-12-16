#pragma once
#include <atomic>

namespace Palette
{
	class IRef
	{
	public:
		void AddRef()
		{
			mRefCount.fetch_add(1, std::memory_order_release);
		}

		void ReleaseRef()
		{
			if (mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
			{
				OnRefDestroy();
			}
		}

	protected:
		IRef() noexcept = default;
		// Delete the copy constructor 
		IRef(const IRef&) noexcept = delete;
		IRef(IRef&&) noexcept = delete;

		// Delete the copy assignment operator
		IRef& operator = (const IRef&) = delete;
		IRef& operator = (IRef&&) noexcept = delete;

		virtual ~IRef() {}

		virtual void OnRefDestroy() { delete this; }

	public:
		template<typename _type> IRef& operator = (_type&&) noexcept = delete;

	private:
		std::atomic<int> mRefCount{ 0 };
	};

	//class IShared : public IRef
	//{
	//
	//};
}
