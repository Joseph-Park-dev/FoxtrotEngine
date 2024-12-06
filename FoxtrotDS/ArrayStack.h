#pragma once
#include "DSLogs.h"

constexpr size_t MAX_STACK_SIZE = 50;
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "../FoxtrotEngine/src/Debugging/DebugMemAlloc.h"

namespace FTDS
{
	template <class TYPE>
	class ArrayStack
	{
	public:
		void Push(TYPE& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			mData[++mTop] = element;
		}

		void Push(TYPE&& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			mData[++mTop] = element;
		}

		TYPE Pop() 
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return mData[mTop--];
		}

		TYPE Peek()
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return mData[mTop];
		}

		void Reserve(size_t newCapacity)
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
				ReAllocate(mCapacity, newCapacity);
			}
			else
				return;
		}

		size_t	Size() { return (mTop + 1); }
		size_t	Capacity() { return mCapacity; }

		void Clear()
		{
			if (mData)
			{
				delete[] mData;
				mData = nullptr;
				mTop = -1;
				mCapacity = 0;
			}
		}

	public:
		bool IsEmpty() { return mTop == -1; }
		bool IsFull()
		{
			if (0 < mCapacity)
				return mTop == mCapacity - 1;
			else
				return false; // Stack with no capacity is not considered as full.
		}

		// Gets the array which stores the data of the stack.
		// This can be used when freeing memory. 
		TYPE* Data() { return mData; }

	public:
		ArrayStack()
			: mData(nullptr)
			, mTop(-1)
			, mCapacity(0)
		{}
		/// <Note on deallocation>
		/// If an ArrayStack consist of dynamically allocated objects,
		/// (which means, variables created with "new" keyword)
		/// its data element must be deleted one by one to prevent memory leak.
		/// Clear() or the destructor won't free those objects automatically.
		/// </Note on deallocation>
		~ArrayStack()
		{
			if (mData)
			{
				delete[] mData;
				mData = nullptr;
			}
		}

	private:
		TYPE*	mData;
		size_t	mTop;
		size_t	mCapacity;

	private:
		// Re-allocate memory space when new capacity is bigger than current capacity
		void ReAllocate(size_t currCap, size_t newCap)
		{
			// Shallow-copy the array elements from mData to copy buffer.
			TYPE* copyBuf = new TYPE[currCap];
			for (size_t i = 0; i < currCap; ++i)
				copyBuf[i] = mData[i];

			// Reset mData with new capacity.
			delete[] mData;
			mData = DBG_NEW TYPE[newCap];

			// Shallow-copy the array elements from copy buffer to mData.
			if (currCap < newCap) // When memory occupation gets bigger.
			{
				for (size_t i = 0; i < currCap; ++i)
					mData[i] = copyBuf[i];
				// For the additional empty memory space, fill them with zero TYPEs.
				for (size_t i = 0; i < newCap - currCap; ++i)
					mData[currCap + i] = NULL;
			}
			delete[] copyBuf;

			// Set the modified capacity.
			mCapacity = newCap;
		}
	};
}