// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Various buffer sizes by data types.
/// </summary>
/// 
#pragma once

namespace Common
{
	namespace BufferSize
	{
		constexpr size_t MAX_CHAR_COUNT = 30;
		constexpr size_t STRING_BUFFER_SIZE = 50 * sizeof(char);
		constexpr size_t WSTRING_BUFFER_SIZE = 50 * sizeof(wchar_t);

		constexpr size_t MAX_FONT_CHAR_COUNT = 256;
	} // namespace BufferSize
} // namespace Common