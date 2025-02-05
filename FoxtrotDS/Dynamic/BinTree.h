#pragma once
#include "BinaryNode.h"

#include "functional"

#include "CircularQueue.h"

constexpr size_t LEVEL_ORDER_Q_SIZE = 100;

namespace FTDS
{
	template <typename TYPE, typename CALLBACK>
	class BinTree
	{
	public:
		// Traversal Algorithms
		void InOrder	(CALLBACK func)
		{
			InOrder(mRoot, func);
		}
		void PreOrder	(CALLBACK func)
		{
			PreOrder(mRoot, func);
		}
		void PostOrder	(CALLBACK func)
		{
			PostOrder(mRoot, func);
		}
		void LevelOrder	(CALLBACK func)
		{
			if (!IsEmpty())
			{
				FTDS::CircularQueue<BinaryNode<TYPE>*> queue;
				queue.Reserve(LEVEL_ORDER_Q_SIZE);
				queue.Enqueue(mRoot);
				while (!queue.IsEmpty())
				{
					BinaryNode<TYPE>* n = queue.Peek();
					queue.Dequeue();
					if (n != nullptr)
					{
						func(n);
						queue.Enqueue(n->Left);
						queue.Enqueue(n->Right);
					}
				}
				
			}
		}

	public:
		BinaryNode<TYPE>* GetRoot() { return mRoot; }
		void		SetRoot(BinaryNode<TYPE>* node) { mRoot = node; }

		bool		IsEmpty() { return mRoot == nullptr; }

	public:
		BinTree() : mRoot(nullptr) {}

	protected:
		BinaryNode<TYPE>* mRoot;

	private:
		void InOrder(BinaryNode<TYPE>* node, CALLBACK f)
		{
			if (node)
			{
				InOrder(node->Left, f);
				f(node);
				InOrder(node->Right, f);
			}
		}
		void PreOrder	(BinaryNode<TYPE>* node, CALLBACK f)
		{
			if (node)
			{
				f(node);
				PreOrder(node->Left, f);
				PreOrder(node->Right, f);
			}
		}
		void PostOrder(BinaryNode<TYPE>* node, CALLBACK f)
		{
			if (node)
			{
				PostOrder(node->Left, f);
				PostOrder(node->Right, f);
				f(node);
			}
		}
	};
}