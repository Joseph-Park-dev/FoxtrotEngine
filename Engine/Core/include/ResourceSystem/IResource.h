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
#include "FileSystem/FileIOHelper.h"

namespace Common
{
	/// @brief String literal keys used for saving resource properties.
	namespace ChunkKey
	{
		namespace FTResource
		{
			constexpr const char* FILE_NAME		= "FileName";
			constexpr const char* RELATIVE_PATH = "Path";
		} // namespace FTResource
	} // namespace ChunkKey

	struct ResourceData
	{
		Common::FTDS::String* FileName;
		Common::FTDS::String* Path;

		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		void SaveProperties(std::ofstream& ofs)
		{
			FileIOHelper::SaveString(ofs, ChunkKey::FTResource::FILE_NAME, FileName);
			FileIOHelper::SaveString(ofs, ChunkKey::FTResource::RELATIVE_PATH, Path);
		}

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		void LoadProperties(std::ifstream& ifs)
		{
			FileIOHelper::LoadBasicString(ifs, *Path);
			FileIOHelper::LoadBasicString(ifs, *FileName);
		}

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
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) = 0;

		/// @brief Loads resource properties into an instance.
		/// @param ifs This should either be a stream from a .chunk file, or from a dedicated resource file
		/// like FTSpriteAnimation, FTPremade, etc.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) = 0;

		////////////////////////////
		/// Accessors / Mutators ///
		////////////////////////////
	public:
		/// @brief Returns the file name used by this iresource.
		/// @return Borrowed access to the file name.
		virtual Common::FTDS::String* GetFileName()		= 0;
		/// @brief Returns the relative path used by this iresource.
		/// @return Borrowed access to the relative path.
		virtual Common::FTDS::String* GetRelativePath() = 0;

		/// @brief Updates the file name used by subsequent operations.
		/// @param val Replacement file name.
		virtual void SetFileName(Common::FTDS::String& val)		= 0;
		/// @brief Updates the relative path used by subsequent operations.
		/// @param val Replacement relative path.
		virtual void SetRelativePath(Common::FTDS::String& val) = 0;

		///////////////////////////////////
		/// Const/Destructors & Copying ///
		///////////////////////////////////
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IResource() = default;
	};
} // namespace Common
