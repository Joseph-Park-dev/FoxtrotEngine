#pragma once
#include <cstdio>

#include "ArrayStack.h"
#include "TreeNode.h"
#include "BinSrchTree.h"

namespace Common
{
	namespace FTDS
	{
		using namespace FTDS;

		template <typename TYPE>
		TreeNode<TYPE>* DFS(BinSrchTree<TYPE>* tree, TYPE val)
		{
			ArrayStack<TreeNode<TYPE>*> stack;
			stack.Reserve(7);
			stack.Push(tree->Root());

			while (!stack.IsEmpty())
			{
				TreeNode<int>* current = stack.Peek();
				if (current->Value == val)
					return current;
				stack.Pop();

				printf("%d ", current->Value);

				if (current->Left)
					stack.Push(current->Left);
				if (current->Right)
					stack.Push(current->Right);
			}
			return nullptr;
		}
	} // namespace FTDS
} // namespace Common