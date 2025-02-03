#pragma once
#include "ArrayDS.h"

namespace FTDS
{
	template <class TYPE>
	class ArrayStack : public FTDS::ArrayDS<TYPE>
	{
		using FTDS::ArrayDS<TYPE>::mData;
		using FTDS::ArrayDS<TYPE>::mCapacity;
		using FTDS::ArrayDS<TYPE>::mSize;

	public:
		virtual void Push(TYPE& element)
		{
			assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
			++mSize;
		}

		virtual void Push(TYPE&& element)
		{
			assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
			++mSize;
		}

		virtual void Pop()
		{
			assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
			--mTop;
			--mSize;
		}

		virtual void Clear() override
		{
			if (this->mData)
			{
				delete[] this->mData;
				this->mData = nullptr;
				mTop = -1;
				this->mSize = 0;
			}
		}

	public:
		TYPE Peek()
		{
			assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
			return this->mData[mTop];
		}

	public:
		ArrayStack()
			: ArrayDS<TYPE>()
			, mTop(-1)
		{}

		virtual ~ArrayStack() override
		{
			if (this->mData)
			{
				delete[] this->mData;
				this->mData = nullptr;
			}
		}

	protected:
		int	mTop;
	};
};