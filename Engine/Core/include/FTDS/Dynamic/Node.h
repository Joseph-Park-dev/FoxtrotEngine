#pragma once
namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		struct Node
		{
			/// @brief Initializes a list value with no successor.
			/// Node template to be used for dynamic data structure.
			/// @param val Value to assign, insert, or process.
			/// @note Initializes the Value base or delegates to its constructor.
			Node(TYPE val)
				: Value(val)
				, Next(nullptr)
			{
			}

			TYPE  Value;
			Node* Next;
		};
	} // namespace FTDS
} // namespace Common
