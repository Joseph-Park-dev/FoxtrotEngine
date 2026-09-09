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
			/// @brief Appends a value at the rear of the queue.
			/// @param value Value to assign, insert, or process.
			void Enqueue(TYPE value)
			{
				mRear		 = (mRear + 1) % mCapacity;
				mData[mRear] = value;

				if (this->IsFull())
					mFront = (mRear + 1) % mCapacity;
				else
					++mSize;
			}
			/// @brief Removes the front queue element.
			void Dequeue()
			{
				assert(!this->IsEmpty());
				mFront = (mFront + 1) % mCapacity;
				--mSize;
			}
			/// @brief Inspects the next accessible element without removing it.
			/// @return Value at the inspected position.
			TYPE Peek()
			{
				assert(!this->IsEmpty());
				return mData[mFront];
			}

		public:
			/// @brief Initializes the circular queue's front and rear indices.
			/// @note Initializes the Array<TYPE> base or delegates to its constructor.
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
