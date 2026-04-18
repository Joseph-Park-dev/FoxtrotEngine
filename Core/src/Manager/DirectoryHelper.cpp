// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DirectoryHelper.h"

#include "Static/FTString.h"

namespace Core
{
	DirectoryHelper::DirectoryHelper()
		: mProjectPath(DBG_NEW FTDS::String())
		, mChunkPath(DBG_NEW FTDS::String())
		, mAssetPath(DBG_NEW FTDS::String())
		, mCurrentChunkSaved(false)
	{
	}

	DirectoryHelper::~DirectoryHelper()
	{
		delete mProjectPath;
		delete mChunkPath;
		delete mAssetPath;
	}

	void DirectoryHelper::RegisterMemberFuncs()
	{
		//GetMembers()->Insert()
	}

	void DirectoryHelper::AbsoluteToRelativePath(FTDS::String& absPath)
	{
		FTDS::String path		= absPath;
		FTDS::String folderName = "\\Assets\\";

		// Check if the path is relative.
		if (path.LFind(".\\") == 0)
			return;

		int index = path.LFind(mAssetPath->C_Str());
		if (index == -1)
			return;

		int cutIndex = path.RFind(folderName.C_Str());
		path.SubStr(cutIndex, path.GetLength());

		FTDS::String result = ".";
		result.Append(path);

		absPath = result;
	}

	void DirectoryHelper::RelativeToAbsolutePath(FTDS::String& relPath)
	{
		FTDS::String path		= relPath;
		FTDS::String folderName = ".\\Assets\\";

		if (path.LFind(".\\") != 0)
			return;

		path.SubStr(folderName.GetLength(), path.GetLength());

		FTDS::String result = *mAssetPath;
		// result.Append("\\");
		result.Append(path);

		relPath = result;
	}

	FTDS::String* DirectoryHelper::GetProjectPath()
	{
		return mProjectPath;
	}

	FTDS::String* DirectoryHelper::GetChunkPath()
	{
		return mChunkPath;
	}

	FTDS::String* DirectoryHelper::GetAssetPath()
	{
		return mAssetPath;
	}

	bool DirectoryHelper::GetCurrChunkSaved() const
	{
		return mCurrentChunkSaved;
	}

	void DirectoryHelper::SetProjectPath(FTDS::String&& path)
	{
		mProjectPath->Assign(path);
		SetAssetPath();
	}

	void DirectoryHelper::SetChunkPath(FTDS::String&& path)
	{
		mChunkPath->Assign(path);
	}

	void DirectoryHelper::SetCurrChunkSaved(bool saved)
	{
		mCurrentChunkSaved = saved;
	}

	void DirectoryHelper::SetAssetPath()
	{
		mAssetPath->Assign(*mProjectPath);
		mAssetPath->Append("\\Assets\\");
	}
} // namespace Core