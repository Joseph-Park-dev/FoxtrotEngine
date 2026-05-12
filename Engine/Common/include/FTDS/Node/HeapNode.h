// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

namespace Common
{
	namespace FTDS
	{
		template <typename TYPE>
		struct HeapNode
		{
			HeapNode(size_t key, const TYPE& value)
				: Key(key), Value(value)
			{
			}

			size_t Key;
			TYPE   Value;
		};
	} // namespace FTDS
} // namespace Common