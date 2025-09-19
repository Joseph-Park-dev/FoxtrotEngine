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
	class Array
	{
	public:
		TYPE& operator[](int idx) { return mData[idx]; }

	public:
		FTDS::FTIteratorArray<TYPE> Begin() { return FTDS::FTIteratorArray<TYPE>(mData); }
		FTDS::FTIteratorArray<TYPE> End() { return FTDS::FTIteratorArray<TYPE>(&mData[mCapacity]); }

		// It is recommended to put null check to mPtr.
		template <class UnaryOperation>
		void IterateArray(
			UnaryOperation&& unaryOp)
		{
			for (size_t i = 0; i < mCapacity; ++i)
				unaryOp(mData[i]);
		}

		void Swap(size_t posLeft, size_t posRight)
		{
			TYPE cache = this->mData[posLeft];
			// Replace the front value with back.
			this->mData[posLeft] = this->mData[posRight];
			// Replace the back value with front.
			this->mData[posRight] = cache;
		}

		void Reverse()
		{
			for (size_t i = 0; i < this->mCapacity / 2; ++i)
				Swap(i, this->mCapacity - 1 - i);
		}

	public:
		// Re-allocate memory space when new capacity is bigger than current capacity
		void Reserve(size_t newCapacity)
		{
			if (newCapacity <= mCapacity)
				return;

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
			if (0 < mCapacity)
			{
				size_t newCap = mCapacity;
				mCapacity	  = 0;

				delete[] mData;
				mData = nullptr;
				AllocateMem(newCap);
			}
		}

	public:
		// Gets the array which stores the data of the stack.
		// This can be used when freeing memory.
		// TYPE*	Data() { return mData; }

		TYPE& At(int idx)
		{
			// assert(mData[idx]);
			return mData[idx];
		}

		size_t Capacity() { return mCapacity; }

	public:
		Array()
			: mData(nullptr)
			, mCapacity(0)
		{
		}

		Array(size_t capacity)
			: mData(nullptr)
			, mCapacity(0)
		{
			Reserve(capacity);
		}

		virtual ~Array()
		{
			if (mData)
			{
				delete[] mData;
				mData = nullptr;
			}
		}

	public:
		const TYPE* Data() const { return mData; }

	protected:
		virtual void AllocateMem(size_t newCap)
		{
			// Create an array with renewed capacity.
			TYPE* newArr = DBG_NEW TYPE[newCap];
			memset(newArr, NULL, sizeof(TYPE) * newCap);

			// Calculate memory size to be copied.
			size_t destSize	  = sizeof(TYPE) * newCap;
			size_t copiedSize = sizeof(TYPE) * mCapacity;

			// Copy previous data.
			memcpy_s(newArr, destSize, mData, copiedSize);
			delete[] mData;

			// Set new array as current data.
			mData = newArr;
			// Set new capacity.
			mCapacity = newCap;
		}

		size_t Min(size_t a, size_t b)
		{
			return a > b ? b : a;
		}

	protected:
		TYPE*  mData;
		size_t mCapacity;
	};
} // namespace FTDS