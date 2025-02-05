#pragma once
#include "BinaryNode.h"
#include "BinTree.h"

namespace FTDS
{
	template <typename TYPE, typename CALLBACK>
	class BinSrchTree : public FTDS::BinTree<TYPE, CALLBACK>
	{
		using BinTree<TYPE, CALLBACK>::mRoot;

	public:
		void InsertNode(int key, TYPE val)
		{
			if (mRoot == nullptr)
			{
				mRoot = new BinaryNode<TYPE>(key, val);
				return;
			}
			InsertNode(mRoot, key, val);
		}

		BinaryNode<TYPE>* FindNode(BinaryNode<TYPE>* current, TYPE target)
		{
			if (!current)
				return nullptr;
			if (current->Value == target)
				return current;
			if (Leftward(current, target))
				return FindNode(current->Left, target);
			if (Rightward(current, target))
				return FindNode(current->Right, target);
			return nullptr;
		}

	private:
		bool Leftward(BinaryNode<TYPE>* curr, TYPE t)
		{
			return t < curr->Value && curr->Left != nullptr;
		}

		bool Rightward(BinaryNode<TYPE>* curr, TYPE t)
		{
			return curr->Value < t && curr->Right != nullptr;
		}

		void InsertNode(BinaryNode<TYPE>* current, int key, TYPE val)
		{
			if (key < current->Key)
			{
				if (current->Left != nullptr)
					InsertNode(current->Left, key, val);
				else
					current->Left = new BinaryNode<TYPE>(key, val);
			}
			else if (current->Key < key)
			{
				if (current->Right != nullptr)
					InsertNode(current->Right, key, val);
				else
					current->Right = new BinaryNode<TYPE>(key, val);
			}
			else // current->Value == val
				return;
		}
	};
}