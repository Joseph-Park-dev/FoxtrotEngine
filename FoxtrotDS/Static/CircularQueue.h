#pragma once
#include <cassert>

#include "ArrayDS.h"

namespace FTDS
{
	template <class TYPE>
	class CircularQueue : public FTDS::ArrayDS<TYPE>
	{
		using FTDS::ArrayDS<TYPE>::mData;
		using FTDS::ArrayDS<TYPE>::mCapacity;
		using FTDS::ArrayDS<TYPE>::mSize;

	public:
		void Enqueue(TYPE value)
		{
			mRear = (mRear + 1) % mCapacity;
			mData[mRear] = value;	

			if (this->IsFull())
				mFront = (mRear + 1) % mCapacity;
			else
				++mSize;
		}
		void Dequeue()
		{
			assert(!this->IsEmpty());
			mFront = (mFront + 1) % mCapacity;
			--mSize;
		}
		TYPE Peek()
		{
			assert(!this->IsEmpty());
			return mData[mFront];
		}

	public:
		CircularQueue()
			: ArrayDS<TYPE>()
			, mFront(0)
			, mRear(-1)
		{}

	private:
		size_t	mFront;
		size_t	mRear;
	};
};