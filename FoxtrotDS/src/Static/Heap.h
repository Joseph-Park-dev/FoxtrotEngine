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
	template <class TYPE>
	class Heap : public FTDS::Array<FTDS::HeapNode<TYPE>*>
	{
	public:
		void Insert(size_t key, TYPE val)
		{
			if (this->mLastIdx == mSize - 1)
				this->Reserve(mSize * 2);
			++mLastIdx;
			++mSize;
			this->mData[this->mLastIdx] = new FTDS::HeapNode<TYPE>(key, val);

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
		Heap()
			: FTDS::Array<FTDS::HeapNode<TYPE>*>()
			, mSize(0)
			, mLastIdx(0)
		{
		}

		~Heap()
		{
			for (size_t i = 0; i < mSize + 1; ++i)
			{
				if (this->mData[i])
					delete this->mData[i];
			}
		}

	private:
		size_t mSize;
		size_t mLastIdx;
	};
} // namespace FTDS