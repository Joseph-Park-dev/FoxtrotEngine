#pragma once
#include "DSLogs.h"
#include "ArrayDS.h"

constexpr size_t MAX_STACK_SIZE = 50;

namespace FTDS
{
	template <class TYPE>
	class ArrayStack : public FTDS::ArrayDS<TYPE>
	{
	public:
		void Push(TYPE& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
		}

		void Push(TYPE&& element)
		{
			assert(!IsFull()); // Stack must not be full. Use Reserve(size_t).
			this->mData[++mTop] = element;
		}

		TYPE Pop()
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return this->mData[mTop--];
		}

		TYPE Peek()
		{
			assert(!IsEmpty()); // Stack must have somthing to pop in itself.
			return this->mData[mTop];
		}

		virtual size_t Size() override { return (mTop + 1); }

		virtual void Clear() override
		{
			if (this->mData)
			{
				delete[] this->mData;
				this->mData = nullptr;
				mTop = -1;
				this->mCapacity = 0;
			}
		}

	public:
		virtual bool IsEmpty() override { return mTop == -1; }
		virtual bool IsFull() override
		{
			if (0 < this->mCapacity)
				return mTop == this->mCapacity - 1;
			else
				return false; // Stack with no capacity is not considered as full.
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

	private:
		size_t	mTop;
	};
};