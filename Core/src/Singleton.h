#pragma once
#include "Debugging/DebugMemAlloc.h"

namespace Core
{
	class ISingleton
	{
	public:
		virtual ~ISingleton() = default;
	};

	template <typename DERIVED>
	class Singleton : public ISingleton
	{
	public:
		static DERIVED* GetInstance()
		{
			if (mInstance == nullptr)
				mInstance = DBG_NEW DERIVED();
			return mInstance;
		}
		static void Destroy()
		{
			if (mInstance)
			{
				delete mInstance;
				mInstance = nullptr;
			}
		}

	protected:
		Singleton() {};
		Singleton(const DERIVED& obj)		   = delete;
		Singleton(const Singleton&)			   = delete;
		Singleton(Singleton&&)				   = delete;
		Singleton& operator=(const Singleton&) = delete;
		Singleton& operator=(Singleton&&)	   = delete;

		static DERIVED* mInstance;
	};

	template <typename T>
	T* Singleton<T>::mInstance = nullptr;
} // namespace Core