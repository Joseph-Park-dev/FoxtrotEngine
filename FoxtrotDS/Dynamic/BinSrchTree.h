#pragma once
#include "TreeNode.h"

namespace FTDS
{
	template <typename TYPE>
	class BinSrchTree
	{
	public:
		void InsertNode(TreeNode* current = mRoot, TYPE val)
		{
			if (current->Value == val)
				return;
			if (val < current->Value)
			{
				if (current->Left != nullptr)
					InsertNode(current->Left, val);
				else
				{
					current->Left = new TreeNode<TYPE>(val);
					current->Left->Parent = current;
				}
			}
			else
			{
				if (current->Right != nullptr)
					InsertNode(current->Right, val);
				else
				{
					current->Right = new TreeNode<TYPE>(val);
					current->Right->Parent = current;
				}
			}
		}

		TreeNode<TYPE>* FindNode(TreeNode* current = mRoot, TYPE target)
		{
			if (!current)
				return NULL;
			if (current->Value == target)
				return current;

			if (Leftward(current, target)
				return FindNode(current->Left, target);
			else if (Rightward(current, target))
				return FindNode(current->Right, target);

			return nullptr;
		}

	private:
		TreeNode* mRoot;

	private:
		bool Leftward(TreeNode* curr, TYPE t)
		{
			return t < curr->Value && curr->Left != nullptr;
		}

		bool Rightward(TreeNode* curr, TYPE t)
		{
			return curr->Value < t && curr->Right != nullptr;
		}
	};
}