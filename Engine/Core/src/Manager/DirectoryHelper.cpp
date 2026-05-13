// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/DirectoryHelper.h"

#include "FTDS/Static/FTString.h"
#include "Plugin/CoreExports.h"

using GET_ASSET_PATH_FUNC = Common::FTDS::String* (*)();

namespace Core
{
	using namespace Common;
	DirectoryHelper::DirectoryHelper()
		: mProjectPath(DBG_NEW Common::FTDS::String())
		, mChunkPath(DBG_NEW Common::FTDS::String())
		, mAssetPath(DBG_NEW Common::FTDS::String())
		, mCurrentChunkSaved(false)
	{
	}

	DirectoryHelper::~DirectoryHelper()
	{
		delete mProjectPath;
		delete mChunkPath;
		delete mAssetPath;
	}

	void DirectoryHelper::AbsoluteToRelativePath(Common::FTDS::String& absPath)
	{
		Common::FTDS::String path		= absPath;
		Common::FTDS::String folderName = "\\Assets\\";

		// Check if the path is relative.
		if (path.LFind(".\\") == 0)
			return;

		int index = path.LFind(mAssetPath->C_Str());
		if (index == -1)
			return;

		int cutIndex = path.RFind(folderName.C_Str());
		path.SubStr(cutIndex, path.GetLength());

		Common::FTDS::String result = ".";
		result.Append(path);

		absPath = result;
	}

	void DirectoryHelper::RelativeToAbsolutePath(Common::FTDS::String& relPath)
	{
		Common::FTDS::String path		= relPath;
		Common::FTDS::String folderName = ".\\Assets\\";

		if (path.LFind(".\\") != 0)
			return;

		path.SubStr(folderName.GetLength(), path.GetLength());

		Common::FTDS::String result = *mAssetPath;
		// result.Append("\\");
		result.Append(path);

		relPath = result;
	}

	Common::FTDS::String* DirectoryHelper::GetProjectPath()
	{
		return mProjectPath;
	}

	Common::FTDS::String* DirectoryHelper::GetChunkPath()
	{
		return mChunkPath;
	}

	Common::FTDS::String* DirectoryHelper::GetAssetPath()
	{
		return mAssetPath;
	}

	bool DirectoryHelper::GetCurrChunkSaved() const
	{
		return mCurrentChunkSaved;
	}

	void DirectoryHelper::SetProjectPath(Common::FTDS::String&& path)
	{
		mProjectPath->Assign(path);
		SetAssetPath();
	}

	void DirectoryHelper::SetChunkPath(Common::FTDS::String&& path)
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

	CORE_API Common::FTDS::String* GetProjectPath()
	{
		return DirectoryHelper::GetInstance()->GetProjectPath();
	}

	CORE_API Common::FTDS::String* Core::GetAssetPath()
	{
		return DirectoryHelper::GetInstance()->GetAssetPath();
	}

	CORE_API Common::FTDS::String* GetChunkPath()
	{
		return DirectoryHelper::GetInstance()->GetChunkPath();
	}

	CORE_API bool GetChunkIsSaved()
	{
		return DirectoryHelper::GetInstance()->GetCurrChunkSaved();
	}

	CORE_API void SetChunkIsSaved(bool val)
	{
		return DirectoryHelper::GetInstance()->SetCurrChunkSaved(val);
	}
} // namespace Core