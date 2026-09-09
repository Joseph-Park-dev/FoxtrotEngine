// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton that manages directories as strings.
/// List of directories
/// 1. Current Project Path
/// 2. Current .Chunk Path
/// In addition, whether a .Chunk is saved or not is also managed here.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "Entity/Entity.h"

#include <filesystem>

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/FTString.h"
#include "Plugin/CoreExports.h"

namespace Core
{
#define PATH_PROJECT DirectoryHelper::GetInstance()->GetProjectPath()
#define PATH_CHUNK DirectoryHelper::GetInstance()->GetChunkPath()
#define CHUNK_IS_SAVED DirectoryHelper::GetInstance()->GetCurrChunkSaved()
#define SET_CHUNK_IS_SAVED(saved) DirectoryHelper::GetInstance()->SetCurrChunkSaved(saved)

	namespace ProcName
	{
		constexpr const char* GetProjectPath  = "GetProjectPath";
		constexpr const char* GetAssetPath	  = "GetAssetPath";
		constexpr const char* GetChunkPath	  = "GetChunkPath";
		constexpr const char* GetChunkIsSaved = "GetChunkIsSaved";
		constexpr const char* SetChunkIsSaved = "SetChunkIsSaved";
	} // namespace ProcName

	using CHUNK_IS_SAVED_FUNC	  = bool (*)();
	using SET_CHUNK_IS_SAVED_FUNC = void (*)(bool);
	using GET_PROJ_PATH_FUNC	  = Common::FTDS::String* (*)();
	using GET_CHUNK_PATH_FUNC	  = Common::FTDS::String* (*)();
	using GET_ASSET_PATH_FUNC	  = Common::FTDS::String* (*)();

	class CORE_API DirectoryHelper
	{
		SINGLETON(DirectoryHelper)

	public:
		// Iterate all files in a folder,
		// takes the folder path & executes unaryOp (Functor).
		/// @brief Visits matching filesystem entries recursively and invokes the supplied callback.
		/// @param dir Directory to enumerate or resolve.
		/// @param unaryOp Callback invoked for each visited entry.
		template <class UnaryOperation>
		static void IterateForFileRecurse(
			const char*		 dir,
			UnaryOperation&& unaryOp)
		{
			for (const std::filesystem::directory_entry& dirEntry :
				 std::filesystem::recursive_directory_iterator(dir))
			{
				if (dirEntry.is_regular_file())
					unaryOp(std::move(dirEntry.path().c_str()));
			}
		}

		/// @brief Rewrites an absolute path relative to the current project directory.
		/// @param absPath abs path used by this operation.
		void AbsoluteToRelativePath(Common::FTDS::String& absPath);
		/// @brief Resolves a project-relative path into an absolute path.
		/// @param relPath Project-relative path to resolve.
		void RelativeToAbsolutePath(Common::FTDS::String& relPath);

	public:
		/// @brief Returns the project path used by this directory helper.
		/// @return Borrowed access to the project path.
		Common::FTDS::String* GetProjectPath();
		/// @brief Returns the chunk path used by this directory helper.
		/// @return Borrowed access to the chunk path.
		Common::FTDS::String* GetChunkPath();
		/// @brief Returns the asset path used by this directory helper.
		/// @return Borrowed access to the asset path.
		Common::FTDS::String* GetAssetPath();
		/// @brief Returns the curr chunk saved used by this directory helper.
		/// @return Current value of the curr chunk saved flag.
		bool				  GetCurrChunkSaved() const;

		/// @brief Updates the project path used by subsequent operations.
		/// @param path Replacement project path.
		void SetProjectPath(Common::FTDS::String&& path);
		/// @brief Updates the chunk path used by subsequent operations.
		/// @param path Replacement chunk path.
		void SetChunkPath(Common::FTDS::String&& path);
		/// @brief Updates the curr chunk saved used by subsequent operations.
		/// @param val Replacement curr chunk saved.
		void SetCurrChunkSaved(bool val);

	private:
		Common::FTDS::String* mProjectPath;
		Common::FTDS::String* mChunkPath;
		Common::FTDS::String* mAssetPath;
		bool				  mCurrentChunkSaved;

	private:
		/// @brief Updates the asset path used by subsequent operations.
		void SetAssetPath();
	};

	extern "C"
	{
		/// @brief Returns the project path used by this service.
		/// @return Borrowed access to the project path.
		CORE_API Common::FTDS::String* GetProjectPath();
		/// @brief Returns the asset path used by this service.
		/// @return Borrowed access to the asset path.
		CORE_API Common::FTDS::String* GetAssetPath();
		/// @brief Returns the chunk path used by this service.
		/// @return Borrowed access to the chunk path.
		CORE_API Common::FTDS::String* GetChunkPath();
		/// @brief Returns the chunk is saved used by this service.
		/// @return Current value of the chunk is saved flag.
		CORE_API bool				   GetChunkIsSaved();
		/// @brief Updates the chunk is saved used by subsequent operations.
		/// @param val Replacement chunk is saved.
		CORE_API void				   SetChunkIsSaved(bool val);
	}
} // namespace Core
