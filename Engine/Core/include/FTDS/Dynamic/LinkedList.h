#pragma once
#include <assert.h>

#include "Node.h"
#include "Debugging/DebugMemAlloc.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class LinkedList
		{
		public:
			/// @brief Adds a value at the insertion end of this container.
			/// @param val Value to assign, insert, or process.
			virtual void Push(TYPE val)
			{
				Node<TYPE>* node = DBG_NEW Node<TYPE>(val);
				if (IsEmpty())
				{
					assert(mHead);
					mHead = node;
				}
				else
				{
					assert(0 < mSize);
					Node<TYPE>* last = GetNode(mSize - 1);
					last->Next		 = node;
				}
				++mSize;
			}

			/// @brief Removes a value from the removal end of this container.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			virtual void Pop()
			{
				assert(!IsEmpty());
				Node<TYPE>* prev = mHead;
				if (1 < mSize)
				{
					// the count 2 = array offset (index 0) +
					// subtraction by 1 to get previous value.
					prev = GetNode(mSize - 2);
					delete prev->Next;
					prev->Next = nullptr;
				}
				else
				{
					delete mHead;
					mHead = nullptr;
				}
				--mSize;
			}

			/// @brief Adds a keyed or positioned element to the container.
			/// @param val Value to assign, insert, or process.
			/// @param idx Zero-based element index.
			virtual void Insert(TYPE val, size_t idx)
			{
				// Given index should be smaller than the number of nodes.
				assert(idx < mSize);
				Node<TYPE>* node = DBG_NEW Node<TYPE>(val);

				Node<TYPE>* prev = GetNode(idx);
				Node<TYPE>* next = prev->Next;

				prev->Next = node;
				node->Next = next;
				++mSize;
			}

			/// @brief Retrieves the last logical element without removing it.
			/// @return Value at the inspected position.
			virtual TYPE Back() { return GetNode(mSize - 1)->Value; }
			/// @brief Inspects the next accessible element without removing it.
			/// @param idx Zero-based element index.
			/// @return Value at the inspected position.
			/// @pre The index must refer to an accessible element.
			virtual TYPE Peek(size_t idx) { return GetNode(idx)->Value; };

			/// @brief Removes the indexed list node and reconnects the surrounding links.
			/// @param idx Zero-based element index.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			void Delete(size_t idx)
			{
				assert(idx < mSize);
				Node<TYPE>* target = mHead;
				if (0 < idx)
				{
					Node<TYPE>* prev = GetNode(idx - 1);
					target			 = prev->Next;
					Node<TYPE>* next = target->Next;

					prev->Next = next;
				}
				else
					mHead = mHead->Next;
				--mSize;
				delete target;
				target = nullptr;
			}

		public:
			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			bool   IsEmpty() { return mSize == 0; }
			/// @brief Exposes the element count used to track occupied storage.
			/// @return Current size.
			size_t Size() { return mSize; }

		public:
			/// @brief Initializes an empty linked list.
			LinkedList<TYPE>()
				: mHead(nullptr)
				, mSize(0)
			{
			}

			/// @brief Releases the resources managed by this instance during destruction.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			~LinkedList<TYPE>()
			{
				for (size_t i = 0; i < mSize; ++i)
				{
					if (mHead)
					{
						Node<TYPE>* next = mHead->Next;
						delete mHead;
						mHead = nullptr;
						mHead = next;
					}
				}
			}

		protected:
			Node<TYPE>* mHead;
			size_t		mSize;

		private:
			/// @brief Returns the node used by this linked list.
			/// @param idx Zero-based element index.
			/// @return Borrowed access to the node.
			Node<TYPE>* GetNode(size_t idx)
			{
				Node<TYPE>* result = mHead;
				for (size_t i = 0; i < idx; ++i)
					result = result->Next;
				return result;
			}
		};
	} // namespace FTDS
} // namespace Common
