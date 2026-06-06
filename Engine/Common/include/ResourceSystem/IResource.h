// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <iosfwd>

#include "FTDS/Static/FTString.h"

namespace Common
{
	struct ResourceData
	{
		Common::FTDS::String* FileName;
		Common::FTDS::String* Path;

#ifdef FOXTROT_EDITOR
		int RefCount;
#endif // FOXTROT_EDITOR
	};

	/// @brief Base class that wraps the resources used in the game.
	class IResource
	{
		/////////////////
		/// Chunk I/O ///
		/////////////////
	public:
		/// @brief Saves resource properties into a file.
		/// @param ofs This should either be a stream to a .chunk file, or to a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void SaveProperties(std::ofstream& ofs) = 0;

		/// @brief Loads resource properties into an instance.
		/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		virtual Common::FTDS::String* GetFileName()		= 0;
		virtual Common::FTDS::String* GetRelativePath() = 0;

		virtual void SetFileName(Common::FTDS::String& val)		= 0;
		virtual void SetRelativePath(Common::FTDS::String& val) = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		virtual ~IResource() = default;
	};

	/// @brief String literal keys used for saving resource properties.
	namespace ChunkKey
	{
		namespace FTResource
		{
			constexpr const char* FILE_NAME		= "FileName";
			constexpr const char* RELATIVE_PATH = "RelativePath";
		} // namespace FTResource
	} // namespace ChunkKey
} // namespace Common