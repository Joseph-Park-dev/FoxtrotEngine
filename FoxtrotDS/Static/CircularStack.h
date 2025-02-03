#pragma once
#include "ArrayStack.h"

namespace FTDS
{
	template <typename TYPE>
	class CircularStack : public FTDS::ArrayStack<TYPE>
	{
		using FTDS::ArrayDS<TYPE>::mData;
		using FTDS::ArrayDS<TYPE>::mCapacity;
		using FTDS::ArrayDS<TYPE>::mSize;
		using FTDS::ArrayStack<TYPE>::mTop;

	public:
		void Push(TYPE& element) override
		{
			assert(0 < mCapacity); // Use Reserve(size_t)
			mTop = (mTop + 1) % static_cast<int>(mCapacity);
			mData[mTop] = element;
			
			if (mSize < mCapacity)
				++mSize;
		}

		void Push(TYPE&& element) override
		{
			assert(0 < mCapacity); // Use Reserve(size_t)
			mTop = (mTop + 1) % static_cast<int>(mCapacity);
			mData[mTop] = element;
			++mSize;
		}

		void Pop() override
		{
			int cap = static_cast<int>(mCapacity);
			mTop = (mTop - 1 + cap) % cap;
			--mSize;
		}

		TYPE Peek()
		{
			assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
			return mData[mTop];
		}

		TYPE Peek(size_t idx)
		{
			assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
			return mData[idx];
		}

		TYPE Front()
		{
			size_t idx = 0;
			if (this->IsFull())
				idx = (mTop + 1) % static_cast<int>(mCapacity)
			return mData[idx];
		}

	public:
		CircularStack()
			: ArrayStack<TYPE>()
		{ }

	private:
		bool IsFull()
		{
			mCapacity <= mSize;
		}
	};
}