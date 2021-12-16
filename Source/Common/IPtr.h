#pragma once
#include <atomic>
#include <assert.h>

namespace Palette
{
	template<typename T> class TSharedPtr
	{
	public:
		TSharedPtr() noexcept = default;
		TSharedPtr(T* ptr)
			: mPtr{ ptr }
		{
			if (mPtr)
				mPtr->AddRef();
		}

		TSharedPtr(const TSharedPtr<T>& rhs) noexcept
		{
			mPtr = rhs.mPtr;
			if (mPtr)
				mPtr->AddRef();
		}

		~TSharedPtr() noexcept
		{
			if (mPtr)
				_Detach();
		}

		T* Get() const noexcept
		{
			return mPtr;
		}

		T* operator -> () const noexcept
		{
			assert(!!mPtr);
			return mPtr;
		}

		TSharedPtr<T>& operator = (T* ptr) noexcept
		{
			if (mPtr)
				_Detach();
			mPtr = ptr;
			if (mPtr)
				mPtr->AddRef();
			return *this;
		}

		TSharedPtr<T>& operator = (const TSharedPtr<T>& rhs) noexcept
		{
			if (mPtr)
				_Detach();
			if (rhs)
			{
				mPtr = rhs.mPtr;
				mPtr->AddRef();
			}
			return *this;
		}

		TSharedPtr<T>& operator = (TSharedPtr<T>&& rhs) noexcept
		{
			if (mPtr)
				_Detach();
			if (rhs)
				_Transfer(std::move(rhs));
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return mPtr != nullptr;
		}

		bool operator ! () const noexcept
		{
			return mPtr == nullptr;
		}

		template<typename T> bool operator == (std::nullptr_t rhs) const noexcept
		{
			return mPtr == rhs;
		}

		template<typename T> bool operator != (std::nullptr_t rhs) const noexcept
		{
			return mPtr != rhs;
		}

		template<typename T> bool operator == (const TSharedPtr<T>& rhs) const noexcept
		{
			return mPtr == rhs.mPtr;
		}

		template<typename T> bool operator != (const TSharedPtr<T>& rhs) const noexcept
		{
			return mPtr != rhs.mPtr;
		}

	protected:
		void _Detach() noexcept
		{
			mPtr->ReleaseRef();
			mPtr = nullptr;
		}

		void _Transfer(TSharedPtr<T>&& src) noexcept
		{
			mPtr = src.mPtr;
			src.mPtr = nullptr;
		}

	public:
		T* mPtr{ nullptr };
	};
}