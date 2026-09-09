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
			/// @brief Adds a keyed or positioned element to the container.
			/// @param key Lookup key identifying the stored entry.
			/// @param val Value to assign, insert, or process.
			void Insert(int key, TYPE val)
			{
				if (mRoot == nullptr)
				{
					mRoot = DBG_NEW BinaryNode<TYPE>(key, val);
					return;
				}
				Insert(mRoot, key, val);
			}

			/// @brief Searches stored entries for the supplied key or value.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed matching entry, or nullptr when the search does not find one.
			BinaryNode<TYPE>* Find(int key)
			{
				if (!mRoot)
					return nullptr;
				else
					return Find(mRoot, key);
			}

			/// @brief Searches the binary search tree by following ordered child links iteratively.
			/// @param key Lookup key identifying the stored entry.
			/// @return Borrowed matching entry, or nullptr when the search does not find one.
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
			/// @brief Tests whether the target key is smaller and a left child exists.
			/// @param curr Current string or value to modify.
			/// @param tKey Target key used to choose a subtree.
			/// @return True when the target key is smaller and a left child exists; otherwise false.
			bool Leftward(BinaryNode<TYPE>* curr, int tKey)
			{
				return tKey < curr->Key && curr->Left;
			}

			/// @brief Tests whether the target key is larger and a right child exists.
			/// @param curr Current string or value to modify.
			/// @param tKey Target key used to choose a subtree.
			/// @return True when the target key is larger and a right child exists; otherwise false.
			bool Rightward(BinaryNode<TYPE>* curr, int tKey)
			{
				return curr->Key < tKey && curr->Right;
			}

			/// @brief Adds a keyed or positioned element to the container.
			/// @param current Current tree node from which traversal begins.
			/// @param key Lookup key identifying the stored entry.
			/// @param val Value to assign, insert, or process.
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

			/// @brief Searches stored entries for the supplied key or value.
			/// @param current Current tree node from which traversal begins.
			/// @param targetKey Key to search for.
			/// @return Borrowed matching entry, or nullptr when the search does not find one.
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
