#pragma once
#include <assert.h>

#include "Node.h"
#include "Debugging/DebugMemAlloc.h"

namespace FTDS
{
	template <typename TYPE>
	class LinkedList 
	{
	public:
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
				last->Next = node;
			}
			++mSize;
		}

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

		virtual void Insert(TYPE val, size_t idx)
		{
			// Given index should be smaller than the number of nodes.
			assert(idx < mSize);  
			Node<TYPE>* node	= DBG_NEW Node<TYPE>(val);

			Node<TYPE>* prev	= GetNode(idx);
			Node<TYPE>* next	= prev->Next;

			prev->Next = node;
			node->Next = next;
			++mSize;
		}

		virtual TYPE Back() { return GetNode(mSize-1)->Value; }
		virtual TYPE Peek(size_t idx) { return GetNode(idx)->Value; };

		void Delete(size_t idx)
		{
			assert(idx < mSize);
			Node<TYPE>* target = mHead;
			if (0 < idx)
			{
				Node<TYPE>* prev = GetNode(idx - 1);
				target = prev->Next;
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
		bool	IsEmpty()	{ return mSize == 0; }
		size_t	Size()		{ return mSize; }

	public:
		LinkedList<TYPE>()
			: mHead(nullptr)
			, mSize(0)
		{}

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
		Node<TYPE>*	mHead;
		size_t		mSize;

	private:
		Node<TYPE>* GetNode(size_t idx) 
		{
			Node<TYPE>* result = mHead;
			for (size_t i = 0; i < idx; ++i)
				result = result->Next;
			return result;
		}
	};
}