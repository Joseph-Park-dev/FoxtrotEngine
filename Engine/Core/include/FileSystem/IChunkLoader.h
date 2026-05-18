// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once
#include <iosfwd>

namespace Core
{
	class IChunkLoader
	{
		// Member Functions for editor level to generate chunk.json files
	public:
		virtual void SaveChunk(const char* chunkPath) = 0;
		virtual void LoadChunk(const char* chunkPath) = 0;

		/// @brief Halts gameloop while loading a .chunk
		virtual void Lock() = 0;

		/// @brief Resume gameloop after finishing .chunk loading.
		virtual void Unlock() = 0;

		/// @brief To prevent .chunk from corrupting, the copy of the file is made before loading.
		/// The copied .chunk is the one that should be read into the game.
		/// @param path The copy is recommended to be located in the same directory with the original.
		/// @return Full path of the copied .chunk
		virtual const char* CopyChunk(const char* chunkPath = "./") = 0;
		/// @brief Delete the copied chunk after being used.
		virtual void DeleteCopiedChunk() = 0;

	protected:
		/// @brief Save .chunk specific data
		virtual void SaveChunkData(::std::ofstream& out) = 0;
		/// @brief Load .chunk specific data
		virtual void LoadChunkData(::std::ifstream& out) = 0;
	};
} // namespace Core