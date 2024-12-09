#pragma once
#include <cassert>

#include "ArrayDS.h"

namespace FTDS
{
	template <class TYPE>
	class CircularQueue : public FTDS::ArrayDS<TYPE>
	{
	public:
		void Enqueue(TYPE value)
		{
			assert(!IsFull());
			mRear = mRear % this->mCapacity;
			this->mData[mRear] = value;
			++mRear;
		}
		TYPE Dequeue()
		{
			assert(!IsEmpty());
			mFront = mFront % this->mCapacity;
			TYPE value = this->mData[mFront];
			++mFront;
			return value;
		}
		TYPE Peek()
		{
			assert(!IsEmpty());
			return this->mData[mFront % this->mCapacity];
		}

	public:
		virtual bool	IsEmpty()	override { return mFront == mRear; }
		virtual bool	IsFull()	override { return (mRear + 1) % (this->mCapacity + 1)  == mFront; }
		virtual size_t	Size()		override { return mRear - mFront; }

	public:
		CircularQueue()
			: ArrayDS<TYPE>()
			, mFront(0)
			, mRear(0)
		{}

	private:
		size_t	mFront;
		size_t	mRear;
	};
};