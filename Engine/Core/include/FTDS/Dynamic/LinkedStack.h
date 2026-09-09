#pragma once
#include "LinkedList.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class LinkedStack :
			public FTDS::LinkedList<TYPE>
		{
			using LinkedList<TYPE>::mHead;
			using LinkedList<TYPE>::mSize;

		public:
			/// @brief Adds a value at the insertion end of this container.
			/// @param val Value to assign, insert, or process.
			virtual void Push(TYPE val) override
			{
				Node<TYPE>* node = DBG_NEW Node<TYPE>(val);
				if (!this->IsEmpty())
				{
					assert(mHead);
					node->Next = mHead;
				}
				mHead = node;
				++mSize;
			}

			/// @brief Removes a value from the removal end of this container.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			virtual void Pop() override
			{
				if (mHead)
				{
					Node<TYPE>* last = mHead;
					mHead			 = mHead->Next;
					delete last;
					last = nullptr;
					--mSize;
				}
			}

			/// @brief Inspects the next accessible element without removing it.
			/// @return Value at the inspected position.
			TYPE& Peek() { return mHead->Value; }

		public:
			/// @brief Initializes an empty linked stack.
			/// @note Initializes the LinkedList<TYPE> base or delegates to its constructor.
			LinkedStack()
				: LinkedList<TYPE>()
			{
			}

			/// @brief Completes destruction through the object's inheritance hierarchy.
			~LinkedStack()
			{
				while (!this->IsEmpty())
					Pop();
			}
		};
	} // namespace FTDS
} // namespace Common
