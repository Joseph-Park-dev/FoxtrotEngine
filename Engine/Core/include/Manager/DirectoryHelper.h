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
		constexpr const char* GetAssetPath = "GetAssetPath";
	} // namespace ProcName

	class DirectoryHelper :
		public Core::Entity
	{
		SINGLETON(DirectoryHelper)

	public:
		// Iterate all files in a folder,
		// takes the folder path & executes unaryOp (Functor).
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

		void AbsoluteToRelativePath(Common::FTDS::String& absPath);
		void RelativeToAbsolutePath(Common::FTDS::String& relPath);

	public:
		Common::FTDS::String* GetProjectPath();
		Common::FTDS::String* GetChunkPath();
		Common::FTDS::String* GetAssetPath();
		bool				  GetCurrChunkSaved() const;

		void SetProjectPath(Common::FTDS::String&& path);
		void SetChunkPath(Common::FTDS::String&& path);
		void SetCurrChunkSaved(bool val);

	private:
		Common::FTDS::String* mProjectPath;
		Common::FTDS::String* mChunkPath;
		Common::FTDS::String* mAssetPath;
		bool				  mCurrentChunkSaved;

	private:
		void SetAssetPath();
	};

	extern "C"
	{
		CORE_API Common::FTDS::String* GetAssetPath();
	}
} // namespace Core