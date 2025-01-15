#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <cassert>

#include "../FoxtrotEngine/src/Debugging/DebugMemAlloc.h"

namespace FTDS
{
	/// <Note_on_deallocation>
	/// If an ArrayDS consist of dynamically allocated objects,
	/// (which means, it has variables created with "new" keyword)
	/// its data element must be deleted one by one to prevent memory leak.
	/// Clear() or the destructor won't free those objects automatically.
	///
	/// Use Capacity() to free the object, DO NOT use Size().
	/// </Note_on_deallocation>
	template <class TYPE>
	class ArrayDS
	{
	public:
		virtual void Reserve(size_t newCapacity)
		{
			assert(0 < newCapacity); // Input capacity must be bigger than Zero.
			// When the current capacity is zero; initialization phase.
			if (mCapacity < 1)
			{
				mData = DBG_NEW TYPE[newCapacity];
				mCapacity = newCapacity;
				return;
			}
			if (mCapacity < newCapacity)
			{
				ReAllocate(newCapacity);
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
		size_t&	Capacity()	{ return mCapacity; }

	public:
		ArrayDS()
			: mData(nullptr)
			, mCapacity(0)
		{}

		virtual ~ArrayDS()
		{
			Clear();
		}

	public:
		virtual bool	IsEmpty()	= 0;
		virtual bool	IsFull()	= 0;
		virtual size_t	Size()		= 0;

	protected:
		TYPE*	mData;
		size_t	mCapacity;

	private:
		void ReAllocate(size_t newCap);
	};

	template<class TYPE>
	// Re-allocate memory space when new capacity is bigger than current capacity
	inline void ArrayDS<TYPE>::ReAllocate(size_t newCap)
	{
		// Shallow-copy the array elements from mData to copy buffer.
		TYPE* copyBuf = new TYPE[mCapacity];
		for (size_t i = 0; i < mCapacity; ++i)
			copyBuf[i] = mData[i];

		// Reset mData with new capacity.
		delete[] mData;
		mData = DBG_NEW TYPE[newCap];

		// Shallow-copy the array elements from copy buffer to mData.
		if (mCapacity < newCap) // When memory occupation gets bigger.
		{
			for (size_t i = 0; i < mCapacity; ++i)
				mData[i] = copyBuf[i];
			// For the additional empty memory space, fill them with zero TYPEs.
			for (size_t i = 0; i < newCap - mCapacity; ++i)
				mData[mCapacity + i] = NULL;
		}
		delete[] copyBuf;

		// Set the modified capacity.
		mCapacity = newCap;
	}
}