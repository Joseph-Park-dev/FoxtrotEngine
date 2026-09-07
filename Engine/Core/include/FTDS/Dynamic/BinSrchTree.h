#pragma once
#include "BinaryNode.h"
#include "BinTree.h"
namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		class BinSrchTree : public FTDS::BinTree<TYPE>
		{
			using BinTree<TYPE>::mRoot;

		public:
			void Insert(int key, TYPE val)
			{
				if (mRoot == nullptr)
				{
					mRoot = DBG_NEW BinaryNode<TYPE>(key, val);
					return;
				}
				Insert(mRoot, key, val);
			}

			BinaryNode<TYPE>* Find(int key)
			{
				if (!mRoot)
					return nullptr;
				else
					return Find(mRoot, key);
			}

			BinaryNode<TYPE>* FindIter(int key)
			{
				BinaryNode<TYPE>* node = mRoot;
				if (node->Key == key)
					return node;

				while (node->Key != key)
				{
					if (Leftward(node, key))
						node = node->Left;
					else if (Rightward(node, key))
						node = node->Right;
					else
						return nullptr;
				}
				return node;
			}

		private:
			bool Leftward(BinaryNode<TYPE>* curr, int tKey)
			{
				return tKey < curr->Key && curr->Left;
			}

			bool Rightward(BinaryNode<TYPE>* curr, int tKey)
			{
				return curr->Key < tKey && curr->Right;
			}

			void Insert(BinaryNode<TYPE>* current, int key, TYPE val)
			{
				if (key < current->Key)
				{
					if (current->Left)
						Insert(current->Left, key, val);
					else
						current->Left = DBG_NEW BinaryNode<TYPE>(key, val);
				}
				else if (current->Key < key)
				{
					if (current->Right)
						Insert(current->Right, key, val);
					else
						current->Right = DBG_NEW BinaryNode<TYPE>(key, val);
				}
				else // current->Value == val
					return;
			}

			BinaryNode<TYPE>* Find(BinaryNode<TYPE>* current, int targetKey)
			{
				if (!current)
					return nullptr;
				if (current->Key == targetKey)
					return current;
				if (Leftward(current, targetKey))
					return Find(current->Left, targetKey);
				if (Rightward(current, targetKey))
					return Find(current->Right, targetKey);
				return nullptr;
			}
		};
	} // namespace FTDS
} // namespace Common