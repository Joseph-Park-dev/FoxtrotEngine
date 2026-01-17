#pragma once
#include "LinkedStack.h"
#include "Node.h"

namespace FTDS
{
	template <typename TYPE>
	class LinkedQueue
	{
	public:
		void Enqueue(TYPE value)
		{
			Node<TYPE>* node = new Node<TYPE>(value);
			if (!mFront)
			{
				mFront = node;
				mRear = node;
			}
			else
			{
				mRear->Next = node;
				mRear = node;
			}
			++mSize;
		}

		void Dequeue()
		{
			if (!mFront)
				return;
			
			Node<TYPE>* front = mFront;
			mFront = mFront->Next;

			if (!mFront)
				mRear = nullptr;

			delete front;
			front = nullptr;

			--mSize;
		}

		TYPE Peek() 
		{ 
			if (mFront) 
				return mFront->Value; 
		}

		size_t Size() { return mSize; }
		bool IsEmpty() { return mSize < 1; }

	public:
		LinkedQueue<TYPE>()
			: mFront(nullptr)
			, mRear(nullptr)
			, mSize(0)
		{}

	private:
		Node<TYPE>* mFront;
		Node<TYPE>* mRear;
		size_t		mSize;
	};
}

#include <stdio.h>

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