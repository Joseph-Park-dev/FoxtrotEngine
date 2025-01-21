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
		void PushBack(TYPE val) 
		{
			Node<TYPE>* node = DBG_NEW Node<TYPE>(val);
			if (IsEmpty())
			{
				assert(!mHead);
				mHead = node;
			}
			else
			{
				assert(0 < mCount);
				Node<TYPE>* last = GetNode(mCount - 1);
				last->Next = node;
			}
			++mCount;
		}

		void Insert(TYPE val, size_t idx)
		{
			// Given index should be smaller than the number of nodes.
			assert(idx < mCount);  
			Node<TYPE>* node	= DBG_NEW Node<TYPE>(val);

			Node<TYPE>* prev	= GetNode(idx);
			Node<TYPE>* next	= prev->Next;

			prev->Next = node;
			node->Next = next;
			++mCount;
		}

		void Pop()
		{
			assert(!IsEmpty());
			Node<TYPE>* prev = mHead;
			if (1 < mCount)
			{
				prev = GetNode(mCount - 2);
				delete prev->Next;
				prev->Next = nullptr;
			}
			else
			{
				delete mHead;
				mHead = nullptr;
			}
			--mCount;
		}

		void Delete(size_t idx)
		{
			assert(idx < mCount);
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
			--mCount;
			delete target;
			target = nullptr;
		}

		TYPE Peek(size_t idx) { return GetNode(idx)->Value; };
		TYPE Back() { return GetNode(mCount-1)->Value; }

	public:
		bool	IsEmpty()	{ return mCount == 0; }
		size_t	GetCount()	{ return mCount; }

	public:
		LinkedList<TYPE>()
			: mHead(nullptr)
			, mCount(0)
		{}

		~LinkedList<TYPE>()
		{
			for (size_t i = 0; i < mCount; ++i)
			{
				Node<TYPE>* next = mHead->Next;
				delete mHead;
				mHead = nullptr;
				mHead = next;
			}
		}

	private:
		Node<TYPE>*	mHead;
		size_t		mCount;

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