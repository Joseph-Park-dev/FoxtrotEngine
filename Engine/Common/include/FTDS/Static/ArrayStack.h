#pragma once
#include "FTDS/Static/Array.h"

namespace Common
{
	namespace FTDS
	{
		template <class TYPE>
		class ArrayStack : public Array<TYPE>
		{
			using Array<TYPE>::mData;
			using Array<TYPE>::mCapacity;

		public:
			virtual void Push(TYPE& element)
			{
				assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
				this->mData[++mTop] = element;
			}

			virtual void Push(TYPE&& element)
			{
				assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
				this->mData[++mTop] = element;
			}

			virtual void Pop()
			{
				assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
				--mTop;
			}

			virtual void Clear() override
			{
				FTDS::Array<TYPE>::Clear();
				mTop = -1;
			}

			bool IsEmpty()
			{
				return mTop < 0;
			}

			bool IsFull()
			{
				return static_cast<int>(mCapacity) - 1 <= mTop;
			}

		public:
			TYPE Peek()
			{
				assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
				return this->mData[mTop];
			}

			size_t GetSize()
			{
				return this->mTop + 1;
			}

		public:
			ArrayStack()
				: Array<TYPE>()
				, mTop(-1)
			{
			}

			ArrayStack(size_t capacity)
				: Array<TYPE>(capacity)
				, mTop(-1)
			{
			}

		private:
			int mTop;
		};
	}; // namespace FTDS
} // namespace Common