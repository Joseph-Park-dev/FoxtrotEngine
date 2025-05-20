#pragma once
#include "BinaryNode.h"
#include "BinTree.h"

namespace FTDS
{
	template <typename TYPE>
	class BinSrchTree : public FTDS::BinTree<TYPE>
	{
		using BinTree<TYPE>::mRoot;

	public:
		void InsertNode(int key, TYPE val)
		{
			if (mRoot == nullptr)
			{
				mRoot = DBG_NEW BinaryNode<TYPE>(key, val);
				return;
			}
			InsertNode(mRoot, key, val);
		}

		TYPE Find(BinaryNode<TYPE>* current, int targetKey)
		{
			if (!current)
				return nullptr;
			if (current->Key == targetKey)
				return current;
			if (Leftward(current, targetKey))
				return FindNode(current->Left, targetKey);
			if (Rightward(current, targetKey))
				return FindNode(current->Right, targetKey);
			return nullptr;
		}

	private:
		bool Leftward(BinaryNode<TYPE>* curr, int tKey)
		{
			return tKey < curr->Key && curr->Left != nullptr;
		}

		bool Rightward(BinaryNode<TYPE>* curr, int tKey)
		{
			return curr->Key < tKey && curr->Right != nullptr;
		}

		void InsertNode(BinaryNode<TYPE>* current, int key, TYPE val)
		{
			if (key < current->Key)
			{
				if (current->Left != nullptr)
					InsertNode(current->Left, key, val);
				else
					current->Left = DBG_NEW BinaryNode<TYPE>(key, val);
			}
			else if (current->Key < key)
			{
				if (current->Right != nullptr)
					InsertNode(current->Right, key, val);
				else
					current->Right = DBG_NEW BinaryNode<TYPE>(key, val);
			}
			else // current->Value == val
				return;
		}
	};
}