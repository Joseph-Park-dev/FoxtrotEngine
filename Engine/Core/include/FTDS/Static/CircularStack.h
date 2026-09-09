#pragma once
#include "ArrayStack.h"

namespace Common
{
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
			/// @brief Adds a value at the insertion end of this container.
			/// @param element Element to insert or edit.
			void Push(TYPE& element) override
			{
				assert(0 < mCapacity); // Use Reserve(size_t)
				mTop		= (mTop + 1) % static_cast<int>(mCapacity);
				mData[mTop] = element;

				if (mSize < mCapacity)
					++mSize;
			}

			/// @brief Adds a value at the insertion end of this container.
			/// @param element Element to insert or edit.
			void Push(TYPE&& element) override
			{
				assert(0 < mCapacity); // Use Reserve(size_t)
				mTop		= (mTop + 1) % static_cast<int>(mCapacity);
				mData[mTop] = element;
				++mSize;
			}

			/// @brief Removes a value from the removal end of this container.
			void Pop() override
			{
				int cap = static_cast<int>(mCapacity);
				mTop	= (mTop - 1 + cap) % cap;
				--mSize;
			}

			/// @brief Inspects the next accessible element without removing it.
			/// @return Value at the inspected position.
			TYPE Peek()
			{
				assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
				return mData[mTop];
			}

		public:
			/// @brief Initializes an empty stack with circular indexing.
			/// @note Initializes the ArrayStack<TYPE> base or delegates to its constructor.
			CircularStack()
				: ArrayStack<TYPE>()
			{
			}

		private:
			/// @brief Tests whether occupied storage has reached the capacity.
			/// @return No defined return value in the current implementation.
			/// @note The current implementation evaluates the fullness condition without returning it.
			bool IsFull()
			{
				mCapacity <= mSize;
			}
		};
	} // namespace FTDS
} // namespace Common
