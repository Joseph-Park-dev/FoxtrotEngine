#pragma once
#include "LinkedList.h"

namespace FTDS
{
    template <typename TYPE>
    class LinkedStack :
        public FTDS::LinkedList<TYPE>
    {
        using LinkedList<TYPE>::mHead;
        using LinkedList<TYPE>::mSize;

    public:
        virtual void Push(TYPE val) override
        {
            Node<TYPE>* node = DBG_NEW Node<TYPE>(val);
            if(!this->IsEmpty())
            {
                assert(mHead);
                node->Next = mHead;
            }
            mHead = node;
            ++mSize;
        }

        virtual void Pop() override
        {
            if (mHead)
            {
                Node<TYPE>* last = mHead;
                mHead = mHead->Next;
                delete last;
                last = nullptr;
                --mSize;
            }
        }

        TYPE Peek() { return mHead->Value; }

    public:
        LinkedStack()
            : LinkedList<TYPE>()
        {}

        ~LinkedStack()
        {
            while (!this->IsEmpty()) Pop();
        }
    };
}