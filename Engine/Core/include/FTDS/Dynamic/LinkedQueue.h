#pragma once
#include "LinkedStack.h"
#include "Node.h"

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class LinkedQueue
		{
		public:
			/// @brief Appends a value at the rear of the queue.
			/// @param value Value to assign, insert, or process.
			void Enqueue(TYPE value)
			{
				Node<TYPE>* node = new Node<TYPE>(value);
				if (!mFront)
				{
					mFront = node;
					mRear  = node;
				}
				else
				{
					mRear->Next = node;
					mRear		= node;
				}
				++mSize;
			}

			/// @brief Removes the front queue element.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			void Dequeue()
			{
				if (!mFront)
					return;

				Node<TYPE>* front = mFront;
				mFront			  = mFront->Next;

				if (!mFront)
					mRear = nullptr;

				delete front;
				front = nullptr;

				--mSize;
			}

			/// @brief Inspects the next accessible element without removing it.
			/// @return Value at the inspected position.
			TYPE Peek()
			{
				if (mFront)
					return mFront->Value;
				return NULL;
			}

			/// @brief Exposes the element count used to track occupied storage.
			/// @return Current size.
			size_t Size() { return mSize; }
			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			bool   IsEmpty() { return mSize < 1; }

		public:
			/// @brief Initializes an empty FIFO queue.
			LinkedQueue<TYPE>()
				: mFront(nullptr)
				, mRear(nullptr)
				, mSize(0)
			{
			}

		private:
			Node<TYPE>* mFront;
			Node<TYPE>* mRear;
			size_t		mSize;
		};
	} // namespace FTDS

#include <stdio.h>

	/// @brief Exercises FIFO insertion, inspection, and removal with dynamically allocated integers.
	inline void Demonstrate_LinkedQueue()
	{
		FTDS::LinkedQueue<int*> queue;

		// Pushing into the queue
		queue.Enqueue(new int(0));
		queue.Enqueue(new int(1));
		queue.Enqueue(new int(2));
		queue.Enqueue(new int(3));
		queue.Enqueue(new int(4));

		// Getting the size of the queue
		size_t size = queue.Size();
		printf("Size : %zu \n", size);

		printf("Elements : \n");
		// Clearing the queue - memory deallocation
		while (!queue.IsEmpty())
		{
			printf("Popping: %d\n", *queue.Peek());
			delete queue.Peek();
			queue.Dequeue();
		}
	}
} // namespace Common
