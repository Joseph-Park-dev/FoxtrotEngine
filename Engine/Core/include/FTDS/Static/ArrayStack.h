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
			/// @brief Adds a value at the insertion end of this container.
			/// @param element Element to insert or edit.
			virtual void Push(TYPE& element)
			{
				assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
				this->mData[++mTop] = element;
			}

			/// @brief Adds a value at the insertion end of this container.
			/// @param element Element to insert or edit.
			virtual void Push(TYPE&& element)
			{
				assert(!this->IsFull()); // Stack must not be full. Use Reserve(size_t).
				this->mData[++mTop] = element;
			}

			/// @brief Removes a value from the removal end of this container.
			virtual void Pop()
			{
				assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
				--mTop;
			}

			/// @brief Resets the logical contents and releases or reinitializes storage as defined by the container.
			virtual void Clear() override
			{
				FTDS::Array<TYPE>::Clear();
				mTop = -1;
			}

			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			bool IsEmpty()
			{
				return mTop < 0;
			}

			/// @brief Tests whether occupied storage has reached the capacity.
			/// @return True when occupied storage has reached the capacity; otherwise false.
			bool IsFull()
			{
				return static_cast<int>(mCapacity) - 1 <= mTop;
			}

		public:
			/// @brief Inspects the next accessible element without removing it.
			/// @return Value at the inspected position.
			TYPE Peek()
			{
				assert(!this->IsEmpty()); // Stack must have somthing to pop in itself.
				return this->mData[mTop];
			}

			/// @brief Returns the size used by this type.
			/// @return Current size.
			size_t GetSize()
			{
				return this->mTop + 1;
			}

		public:
			/// @brief Initializes an empty stack over contiguous storage.
			/// @note Initializes the Array<TYPE> base or delegates to its constructor.
			ArrayStack()
				: Array<TYPE>()
				, mTop(-1)
			{
			}

			/// @brief Initializes an empty stack over contiguous storage.
			/// @param capacity Initial number of element slots to allocate.
			/// @note Initializes the Array<TYPE> base or delegates to its constructor.
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
