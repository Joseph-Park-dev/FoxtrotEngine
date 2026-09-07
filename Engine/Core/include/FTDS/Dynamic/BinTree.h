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
			template <typename FUNC>
			void InOrder(FUNC func)
			{
				InOrder(mRoot, func);
			}

			template <typename FUNC>
			void PreOrder(FUNC func)
			{
				PreOrder(mRoot, func);
			}

			template <typename FUNC>
			void PostOrder(FUNC func)
			{
				PostOrder(mRoot, func);
			}

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
			BinaryNode<TYPE>* GetRoot() { return mRoot; }
			void			  SetRoot(BinaryNode<TYPE>* node) { mRoot = node; }

			bool IsEmpty() { return mRoot == nullptr; }

		public:
			BinTree()
				: mRoot(nullptr) {}

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