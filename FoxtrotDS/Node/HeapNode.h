#pragma once

namespace FTDS
{
	template <typename TYPE>
	struct HeapNode
	{
		HeapNode(size_t key, TYPE value)
			: Key(key), Value(value)
		{
		}

		size_t Key;
		TYPE   Value;
	};
} // namespace FTDS