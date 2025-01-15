#pragma once
namespace FTDS
{
	template <typename TYPE>
	struct Node
	{
		/// <summary>
		/// Node template to be used for dynamic data structure.
		/// </summary>
		Node(TYPE val)
			: Value(val)
			, Next(nullptr)
		{}

		TYPE	Value;
		Node*	Next;
	};
}