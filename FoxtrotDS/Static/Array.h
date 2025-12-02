// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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
		virtual FTDS::FTIteratorArray<TYPE> Begin() { return FTDS::FTIteratorArray<TYPE>(mData); }
		virtual FTDS::FTIteratorArray<TYPE> End() { return FTDS::FTIteratorArray<TYPE>(&mData[mCapacity]); }

		// It is recommended to put null check to mPtr.
		template <class Func>
		void IterateArray(
			Func&& unaryOp)
		{
			for (size_t i = 0; i < mCapacity; ++i)
				unaryOp(mData[i]);
		}

		template <class Func>
		void IterateArray(
			Func&& unaryOp, size_t& currPos)
		{
			currPos = 0;
			for (size_t i = 0; i < mCapacity; ++i)
			{
				unaryOp(mData[i]);
				currPos = i;
			}
		}

	public:
		/**
		 * @brief Construct an empty heap.
		 *
		 * Initializes internal counters and relies on the base `FTDS::Array` constructor
		 * to set up the underlying storage.
		 */
		Heap()
			: FTDS::Array<FTDS::HeapNode<TYPE>*>()
			, mSize(0)
			, mLastIdx(0)
		{
		}

		/**
		 * @brief Destructor - frees owned `HeapNode` pointers.
		 *
		 * Iterates through the underlying array up to `mSize + 1` (accounts for 1-based indexing)
		 * and deletes each non-null pointer. After this destructor runs, no `HeapNode` pointers
		 * owned by this heap remain allocated.
		 *
		 * @note Ensure no other owner will attempt to use or delete the nodes after the heap is destroyed.
		 */
		~Heap()
		{
			for (size_t i = 0; i < mSize + 1; ++i)
			{
				if (this->mData[i])
					delete this->mData[i];
			}
		}

	private:
		/**
		 * @brief Number of elements currently stored in the heap.
		 *
		 * Logical count of elements. For this implementation (1-based indexing), `mSize`
		 * represents the number of valid heap elements.
		 */
		size_t mSize;

		/**
		 * @brief Last used index in the underlying array.
		 *
		 * Under 1-based indexing, `mLastIdx` points to the index of the most recently inserted element.
		 * Typically, `mLastIdx == mSize` after an insertion.
		 */
		size_t mLastIdx;
	};
} // namespace FTDS