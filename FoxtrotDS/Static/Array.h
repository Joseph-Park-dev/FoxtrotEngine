#pragma once
#include <memory>
#include <cassert>

#include "Iterator/FTIterator.h"
#include "Debugging/DebugMemAlloc.h"
#include "Debugging/DebugFuncs.h"

namespace FTDS
{
	/// <Note_on_deallocation>
	/// If an ArrayDS consist of dynamically allocated objects,
	/// (which means, it has variables created with "new" keyword)
	/// its data element must be deleted one by one to prevent memory leak.
	/// Clear() or the destructor won't free those objects automatically.
	/// </Note_on_deallocation>
	template <typename TYPE>
	class Array : public FTDS::FTIterator<TYPE>
	{
	public:
		TYPE& operator*() override { return *mPtr; }
		TYPE* operator++() override
		{
			++mPtr;
			return mPtr;
		}
		TYPE* operator++(int) override
		{
			TYPE tmp = (*mPtr);
			++mPtr;
			return &tmp;
		}

		bool  operator==(Array&& rhs) { return mPtr == rhs.mPtr; }
		bool  operator!=(Array&& rhs) { return mPtr != rhs.mPtr; }
		TYPE& operator[](int idx) { return mData[idx]; }

	public:
		TYPE*  Begin() { return &mData[0]; }
		TYPE*  End() { return &mData[mCapacity]; }
		size_t IterPos() { return mPtr - Begin(); }

	public:
		void Reserve(size_t newCapacity)
		{
			if (newCapacity < 1) // Input capacity must be bigger than Zero.
			{
				Debug::LogError(__LINE__, __FILE__, "New capacity is 0!");
				return;
			}
			// When the current capacity is zero; initialization phase.
			AllocateMem(newCapacity);
		}

		// Clears the data, leaving the capacity unchanged.
		virtual void Clear()
		{
			if (mData)
				memset(mData, NULL, sizeof(mData));
		}

	public:
		// Gets the array which stores the data of the stack.
		// This can be used when freeing memory.
		// TYPE*	Data() { return mData; }

		TYPE& At(int idx)
		{
			assert(mData[idx]);
			return mData[idx];
		}

		size_t Capacity() { return mCapacity; }

	public:
		Array()
			: mData(nullptr)
			, mCapacity(0)
			, mPtr(nullptr)
		{
		}

		Array(size_t capacity)
			: mData(nullptr)
			, mCapacity(0)
			, mPtr(nullptr)
		{
			Reserve(capacity);
		}

		virtual ~Array()
		{
			delete[] mData;
		}

	public:
		TYPE* Data() { return mData; }

	protected:
		TYPE*  mData;
		size_t mCapacity;

	private:
		TYPE* mPtr;

	private:
		// Re-allocate memory space when new capacity is bigger than current capacity
		void AllocateMem(size_t newCap)
		{
			if (newCap < mCapacity)
				return;

			// Create an array with renewed capacity.
			TYPE* newArr = DBG_NEW TYPE[newCap];
			for (size_t i = 0; i < newCap; ++i)
				newArr[i] = NULL;

			// Copy previous data.
			memcpy_s(newArr, sizeof(newArr), mData, sizeof(TYPE) * mCapacity);
			delete[] mData;

			// Set new array as current data.
			mData = newArr;
			// Set new capacity.
			mCapacity = newCap;
		}
	};
} // namespace FTDS