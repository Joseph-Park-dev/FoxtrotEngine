#pragma once
#include <cassert>

#include <Static/Array.h>

namespace Common
{
	namespace FTDS
	{
		template <class TYPE>
		class CircularQueue : public FTDS::Array<TYPE>
		{
			using FTDS::Array<TYPE>::mData;
			using FTDS::Array<TYPE>::mCapacity;
			using FTDS::Array<TYPE>::mSize;

		public:
			void Enqueue(TYPE value)
			{
				mRear		 = (mRear + 1) % mCapacity;
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
				: Array<TYPE>()
				, mFront(0)
				, mRear(-1)
			{
			}

		private:
			size_t mFront;
			size_t mRear;
		};
	}; // namespace FTDS
} // namespace Common