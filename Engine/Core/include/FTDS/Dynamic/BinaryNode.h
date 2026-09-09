#pragma once
namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		struct BinaryNode
		{
			/// @brief Initializes a keyed tree node with no children.
			/// @param key Lookup key identifying the stored entry.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the Key base or delegates to its constructor.
			BinaryNode(int key, TYPE val)
				: Key(key)
				, Value(val)
				, Left(nullptr)
				, Right(nullptr)
			{
			}

			/// @brief Tests whether this tree node has neither a left nor a right child.
			/// @return True when this tree node has neither a left nor a right child; otherwise false.
			bool IsLeaf() { return Left == nullptr && Right == nullptr; }

			int	 Key;
			TYPE Value;

			BinaryNode* Left;
			BinaryNode* Right;
		};
	} // namespace FTDS
} // namespace Common
