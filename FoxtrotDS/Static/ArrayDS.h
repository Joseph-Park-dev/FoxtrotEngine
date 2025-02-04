#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cassert>

#include "Debugging/DebugMemAlloc.h"

namespace FTDS
{
	/// <Note_on_deallocation>
	/// If an ArrayDS consist of dynamically allocated objects,
	/// (which means, it has variables created with "new" keyword)
	/// its data element must be deleted one by one to prevent memory leak.
	/// Clear() or the destructor won't free those objects automatically.
	/// </Note_on_deallocation>
	template <class TYPE>
	class ArrayDS
	{
	public:
		virtual void Reserve(size_t newSize)
		{
			assert(0 < newSize); // Input capacity must be bigger than Zero.
			// When the current capacity is zero; initialization phase.
			if (mCapacity < 1)
			{
				mData = DBG_NEW TYPE[newSize];
				mCapacity = newSize;
				return;
			}
			else if (mCapacity < newSize)
			{
				ReAllocate(newSize);
			}
			else
				return;
		}

		virtual void Clear()
		{
			if (mData)
			{
				delete[] mData;
				mData = nullptr;
			};
		}

	public:
		// Gets the array which stores the data of the stack.
		// This can be used when freeing memory. 
		TYPE*	Data()		{ return mData; }
		size_t	Size()		{ return mSize; }
		size_t	Capacity()	{ return mCapacity; }

	public:
		ArrayDS()
			: mData(nullptr)
			, mSize(0)
			, mCapacity(0)
		{}

		virtual ~ArrayDS()
		{
			Clear();
		}

	public:
		virtual bool IsEmpty()	{ return mSize == 0; }
		virtual bool IsFull()	{ return mCapacity <= mSize; }

	protected:
		TYPE*	mData;
		size_t	mSize;
		size_t  mCapacity;

	private:
		void ReAllocate(size_t newSize);
	};

	template<class TYPE>
	// Re-allocate memory space when new capacity is bigger than current capacity
	inline void ArrayDS<TYPE>::ReAllocate(size_t newCap)
	{
		// Create an array with renewed capacity.
		TYPE* newArr = new TYPE[newCap];

		// Copy existing data to new array.
		for (size_t i = 0; i < mSize; ++i)
			newArr[i] = mData[i];

		// Free existing memory space & set current data to new array.
		delete[] mData;
		mData = newArr;

		// Set the modified capacity.
		mCapacity = newCap;
	}
}