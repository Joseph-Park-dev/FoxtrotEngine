#pragma once
namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		struct BinaryNode
		{
			BinaryNode(int key, TYPE val)
				: Key(key)
				, Value(val)
				, Left(nullptr)
				, Right(nullptr)
			{
			}

			bool IsLeaf() { return Left == nullptr && Right == nullptr; }

			int	 Key;
			TYPE Value;

			BinaryNode* Left;
			BinaryNode* Right;
		};
	} // namespace FTDS
} // namespace Common