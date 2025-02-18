#pragma once
#include <cassert>
#include <memory>

#include "Debugging/DebugMemAlloc.h"

namespace FTDS
{
	/// <Note_on_deallocation>
	/// If an ArrayDS consist of dynamically allocated objects,
	/// (which means, it has variables created with "new" keyword)
	/// its data element must be deleted one by one to prevent memory leak.
	/// Clear() or the destructor won't free those objects automatically.
	/// </Note_on_deallocation>
	template <typename TYPE>
	class ArrayDS
	{
	public:
		void Reserve(size_t newCapacity)
		{
			assert(0 < newCapacity); // Input capacity must be bigger than Zero.
			// When the current capacity is zero; initialization phase.
			AllocateMem(newCapacity);
		}

		// Clears the data, leaving the capacity unchanged.
		virtual void Clear()
		{
			if (mData)
			{
				memset(mData, NULL, sizeof(mData));
				mSize = 0;
			}
		}

	public:
		virtual bool IsEmpty() { return mSize == 0; }
		virtual bool IsFull() { return mCapacity <= mSize; }

	public:
		// Gets the array which stores the data of the stack.
		// This can be used when freeing memory.
		TYPE*  Data() { return mData; }
		size_t Size() { return mSize; }
		size_t Capacity() { return mCapacity; }

	public:
		ArrayDS()
			: mData(nullptr)
			, mSize(0)
			, mCapacity(0)
		{
		}

		ArrayDS(size_t capacity)
			: mData(nullptr)
			, mSize(0)
			, mCapacity(0)
		{
			Reserve(capacity);
		}

		virtual ~ArrayDS()
		{
			delete[] mData;
		}

	protected:
		TYPE*  mData;
		size_t mSize;
		size_t mCapacity;

	private:
		// Re-allocate memory space when new capacity is bigger than current capacity
		void AllocateMem(size_t newCap)
		{
			if (newCap < mCapacity)
				return;

			// Create an array with renewed capacity.
			TYPE* newArr = new TYPE[newCap];

			// Copy previous data.
			memcpy_s(newArr, sizeof(newArr), mData, sizeof(TYPE) * mSize);
			delete[] mData;

			// Set new array as current data.
			mData = newArr;
			// Set new capacity.
			mCapacity = newCap;
		}
	};
} // namespace FTDS