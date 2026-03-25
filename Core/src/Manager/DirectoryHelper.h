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
#include <filesystem>

#include "SingletonMacro.h"
#include "Static/FTString.h"

#define PATH_PROJECT DirectoryHelper::GetInstance()->GetProjectPath()
#define PATH_CHUNK DirectoryHelper::GetInstance()->GetChunkPath()
#define CHUNK_IS_SAVED DirectoryHelper::GetInstance()->GetCurrChunkSaved()
#define SET_CHUNK_IS_SAVED(saved) DirectoryHelper::GetInstance()->SetCurrChunkSaved(saved);

class DirectoryHelper
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
				unaryOp(std::move(dirEntry.path().string()));
		}
	}

	void AbsoluteToRelativePath(FTDS::String& absPath);
	void RelativeToAbsolutePath(FTDS::String& relPath);

public:
	FTDS::String* GetProjectPath();
	FTDS::String* GetChunkPath();
	FTDS::String* GetAssetPath();
	bool		  GetCurrChunkSaved() const;

	void SetProjectPath(FTDS::String&& path);
	void SetChunkPath(FTDS::String&& path);
	void SetCurrChunkSaved(bool val);

private:
	FTDS::String* mProjectPath;
	FTDS::String* mChunkPath;
	FTDS::String* mAssetPath;
	bool		  mCurrentChunkSaved;

private:
	void SetAssetPath();
};