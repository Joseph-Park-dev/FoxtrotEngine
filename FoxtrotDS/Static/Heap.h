// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <Static/Array.h>

#include <climits>

#include <Node/HeapNode.h>
#include <Math/FTMath.h>

namespace FTDS
{
	/**
	 * @brief A simple max-heap container that stores pointers to `FTDS::HeapNode<TYPE>`.
	 *
	 * The `Heap` template inherits from `FTDS::Array<FTDS::HeapNode<TYPE>*>` and manages
	 * ownership of each allocated `HeapNode<TYPE>` stored in the underlying array.
	 *
	 * Notes and conventions:
	 * - The heap implements a binary max-heap: parent node's key >= children's keys.
	 * - Indexing convention: the heap uses 1-based arithmetic for parent/child calculations
	 *   (i.e. parent(index) == floor(index / 2)) which implies element 0 is reserved/unused
	 *   (consistent with the implementation's use of `parent >= 1` in sifting logic).
	 * - Ownership: the `Heap` is responsible for deleting allocated `HeapNode<TYPE>*` entries
	 *   in its destructor.
	 *
	 * Template parameter:
	 * - TYPE: the payload type stored inside `FTDS::HeapNode<TYPE>`.
	 */
	template <class TYPE>
	class Heap : public FTDS::Array<FTDS::HeapNode<TYPE>*>
	{
	public:
		/**
		 * @brief Insert a new key/value pair into the heap.
		 *
		 * Allocates a new `FTDS::HeapNode<TYPE>` with the given `key` and `val`, appends it
		 * to the internal array, and performs the sift-up operation to restore the max-heap
		 * property.
		 *
		 * Behavior details:
		 * - If the underlying array is full (`mLastIdx == mSize - 1`), the array is resized
		 *   by calling `Reserve(mSize * 2)`. Resizing cost is amortized across insertions.
		 * - The newly created node is appended at index `mLastIdx + 1`, then sifting is
		 *   performed while the parent exists and has a smaller key.
		 * - Uses `DBG_NEW` to allocate the node (preserves existing debug allocation macros).
		 *
		 * Complexity:
		 * - Average / amortized: O(log n) (sift-up).
		 * - Worst-case when resizing occurs: reallocations may incur additional O(n) cost.
		 *
		 * Ownership:
		 * - The heap takes ownership of the allocated `HeapNode<TYPE>*`. The destructor deletes
		 *   all stored nodes.
		 *
		 * @param key The priority key used for ordering in the max-heap (larger == higher priority).
		 * @param val The payload value to store in the node.
		 */
		void Insert(size_t key, TYPE val)
		{
			if (this->mLastIdx == mSize - 1)
				this->Reserve(mSize * 2);
			++mLastIdx;
			++mSize;
			this->mData[this->mLastIdx] = DBG_NEW FTDS::HeapNode<TYPE>(key, val);

			// Allocate new node to heap.
			size_t current = mLastIdx;
			size_t parent  = floor(current / 2);
			while (parent >= 1 && this->mData[parent]->Key < this->mData[current]->Key)
			{
				this->Swap(current, parent);
				current = parent;
				parent	= floor(current / 2);
			}
		}

	public:
		/**
		 * @brief Default constructor.
		 *
		 * Initializes the base `FTDS::Array` and sets internal counters to represent an empty heap.
		 * Invariant after construction:
		 * - `mSize == 0`
		 * - `mLastIdx == 0`
		 *
		 * Note: The underlying `FTDS::Array` constructor handles initial allocation state.
		 */
		Heap()
			: FTDS::Array<FTDS::HeapNode<TYPE>*>()
			, mSize(0)
			, mLastIdx(0)
		{
		}

		/**
		 * @brief Destructor — releases owned `HeapNode` pointers.
		 *
		 * Iterates through the internal storage and deletes any non-null `HeapNode<TYPE>*`.
		 * The loop iterates from index 0 to `mSize` inclusive to match the allocation/usage
		 * pattern employed by this heap implementation.
		 *
		 * Safety:
		 * - Guard against null pointers before deletion.
		 * - Assumes all stored pointers were allocated with `new` (or via `DBG_NEW` macro).
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
		 * Represents the logical number of elements. Note the implementation uses `mLastIdx`
		 * as the index into the underlying array where the last element resides; `mSize`
		 * is maintained in parallel to describe how many valid entries exist.
		 */
		size_t mSize;

		/**
		 * @brief Index of the last element in the underlying array.
		 *
		 * This value is used for append/sift operations. Because the heap uses 1-based
		 * parent/child arithmetic, `mLastIdx` will be 0 for an empty heap and increases
		 * as elements are inserted.
		 */
		size_t mLastIdx;
	};
} // namespace FTDS