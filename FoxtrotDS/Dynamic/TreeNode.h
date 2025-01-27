#pragma once

template <typename TYPE>
struct TreeNode
{
	TreeNode(TYPE val)
		: Value	(val)
		, Left	(nullptr)
		, Right	(nullptr)
		, Parent(nullptr)
	{}

	TYPE Value;
	TreeNode* Left;
	TreeNode* Right;
	TreeNode* Parent;
};