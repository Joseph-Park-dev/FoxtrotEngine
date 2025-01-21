#pragma once
#include "ArrayDS.h"

namespace FTDS
{
	template <class TYPE>
	class ArrayStack : public FTDS::ArrayDS<TYPE>
	{
	public:
		virtual void Push(TYPE& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
		}

		virtual void Push(TYPE&& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
		}

		virtual TYPE Pop()
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return this->mData[mTop--];
		}

		virtual TYPE Peek()
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return this->mData[mTop];
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
		virtual bool IsEmpty()	override { return mTop == -1; }
		virtual bool IsFull()	override { return mTop == this->mSize - 1; }

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