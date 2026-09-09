#pragma once
#include "BinaryNode.h"

#include "functional"

#include "FTDS/Static/CircularQueue.h"

namespace Common
{
	constexpr size_t LEVEL_ORDER_Q_SIZE = 100;

	namespace FTDS
	{
		template <typename TYPE>
		class BinTree
		{
		public:
			// Traversal Algorithms
			/// @brief Visits tree nodes in left-subtree, node, right-subtree order.
			/// @param func Callback invoked for each visited entry.
			template <typename FUNC>
			void InOrder(FUNC func)
			{
				InOrder(mRoot, func);
			}

			/// @brief Visits each tree node before its left and right subtrees.
			/// @param func Callback invoked for each visited entry.
			template <typename FUNC>
			void PreOrder(FUNC func)
			{
				PreOrder(mRoot, func);
			}

			/// @brief Visits each tree node after its left and right subtrees.
			/// @param func Callback invoked for each visited entry.
			template <typename FUNC>
			void PostOrder(FUNC func)
			{
				PostOrder(mRoot, func);
			}

			/// @brief Visits tree nodes breadth first using a bounded temporary queue.
			/// @param func Callback invoked for each visited entry.
			template <typename FUNC>
			void LevelOrder(FUNC func)
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
			/// @brief Returns the root used by this bin tree.
			/// @return Borrowed access to the root.
			BinaryNode<TYPE>* GetRoot() { return mRoot; }
			/// @brief Updates the root used by subsequent operations.
			/// @param node Replacement root.
			void			  SetRoot(BinaryNode<TYPE>* node) { mRoot = node; }

			/// @brief Tests whether the container has no logical elements.
			/// @return True when the container has no logical elements; otherwise false.
			bool IsEmpty() { return mRoot == nullptr; }

		public:
			/// @brief Initializes an empty binary tree.
			BinTree()
				: mRoot(nullptr) {}

			/// @brief Releases the resources managed by this instance during destruction.
			/// @note Releasing container storage does not implicitly delete objects held through raw pointer values.
			~BinTree()
			{
				PostOrder([](BinaryNode<TYPE>* node) {
					delete node;
					node = nullptr;
				});
			}

		protected:
			BinaryNode<TYPE>* mRoot;

		private:
			/// @brief Visits tree nodes in left-subtree, node, right-subtree order.
			/// @param node Tree or list node used by the operation.
			/// @param f Callback applied to each visited tree node.
			template <typename FUNC>
			void InOrder(BinaryNode<TYPE>* node, FUNC f)
			{
				if (node)
				{
					InOrder(node->Left, f);
					f(node);
					InOrder(node->Right, f);
				}
			}

			/// @brief Visits each tree node before its left and right subtrees.
			/// @param node Tree or list node used by the operation.
			/// @param f Callback applied to each visited tree node.
			template <typename FUNC>
			void PreOrder(BinaryNode<TYPE>* node, FUNC f)
			{
				if (node)
				{
					f(node);
					PreOrder(node->Left, f);
					PreOrder(node->Right, f);
				}
			}

			/// @brief Visits each tree node after its left and right subtrees.
			/// @param node Tree or list node used by the operation.
			/// @param f Callback applied to each visited tree node.
			template <typename FUNC>
			void PostOrder(BinaryNode<TYPE>* node, FUNC f)
			{
				if (node)
				{
					PostOrder(node->Left, f);
					PostOrder(node->Right, f);
					f(node);
				}
			}
		};
	} // namespace FTDS
} // namespace Common
