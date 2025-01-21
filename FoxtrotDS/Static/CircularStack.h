#pragma once
#include "ArrayStack.h"

#include  <stdexcept>

namespace FTDS
{
	template <class TYPE>
	class CircularStack : public ArrayStack<TYPE>
	{
	public:
		virtual void Push(TYPE& element) override
		{
			if (this->IsFull())
				this->mTop = 0; // Wrap around to the start of the stack
			else
				++this->mTop;
			this->mData[this->mTop] = element;
		}

		virtual void Push(TYPE&& element) override
		{
			if (this->IsFull())
				this->mTop = 0; // Wrap around to the start of the stack
			else
				++this->mTop;
			this->mData[this->mTop] = element;
		}

		virtual TYPE Pop() override
		{
			if (this->IsEmpty()) 
				throw std::out_of_range("Stack is empty");
			TYPE value = this->mData[this->mTop];

			if (this->mTop == 0)
				this->mTop = this->mSize - 1; // Wrap around to the end of the stack
			else
				--this->mTop;

			return value;
		}

		virtual TYPE Peek() override
		{
			if (this->IsEmpty())
				throw std::out_of_range("Stack is empty");
			return this->mData[this->mTop];
		}

		int Top() { return this->mTop; }

	public:
		CircularStack()
			: ArrayStack<TYPE>()
		{}
	};
};