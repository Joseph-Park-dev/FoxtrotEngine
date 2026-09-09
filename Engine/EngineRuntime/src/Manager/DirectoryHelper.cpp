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
	/// @brief Initializes project, asset, and chunk path state.
	/// @note Initializes the :DirectoryHelper base or delegates to its constructor.
	DirectoryHelper::DirectoryHelper()
		: mProjectPath(DBG_NEW Common::FTDS::String())
		, mChunkPath(DBG_NEW Common::FTDS::String())
		, mAssetPath(DBG_NEW Common::FTDS::String())
		, mCurrentChunkSaved(false)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	DirectoryHelper::~DirectoryHelper()
	{
		delete mProjectPath;
		delete mChunkPath;
		delete mAssetPath;
	}

	/// @brief Rewrites an absolute path relative to the current project directory.
	/// @param absPath abs path used by this operation.
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

	/// @brief Resolves a project-relative path into an absolute path.
	/// @param relPath Project-relative path to resolve.
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

	/// @brief Returns the project path used by this directory helper.
	/// @return Borrowed access to the project path.
	Common::FTDS::String* DirectoryHelper::GetProjectPath()
	{
		return mProjectPath;
	}

	/// @brief Returns the chunk path used by this directory helper.
	/// @return Borrowed access to the chunk path.
	Common::FTDS::String* DirectoryHelper::GetChunkPath()
	{
		return mChunkPath;
	}

	/// @brief Returns the asset path used by this directory helper.
	/// @return Borrowed access to the asset path.
	Common::FTDS::String* DirectoryHelper::GetAssetPath()
	{
		return mAssetPath;
	}

	/// @brief Returns the curr chunk saved used by this directory helper.
	/// @return Current value of the curr chunk saved flag.
	bool DirectoryHelper::GetCurrChunkSaved() const
	{
		return mCurrentChunkSaved;
	}

	/// @brief Updates the project path used by subsequent operations.
	/// @param path Replacement project path.
	void DirectoryHelper::SetProjectPath(Common::FTDS::String&& path)
	{
		mProjectPath->Assign(path);
		SetAssetPath();
	}

	/// @brief Updates the chunk path used by subsequent operations.
	/// @param path Replacement chunk path.
	void DirectoryHelper::SetChunkPath(Common::FTDS::String&& path)
	{
		mChunkPath->Assign(path);
	}

	/// @brief Updates the curr chunk saved used by subsequent operations.
	/// @param saved Replacement curr chunk saved.
	void DirectoryHelper::SetCurrChunkSaved(bool saved)
	{
		mCurrentChunkSaved = saved;
	}

	/// @brief Updates the asset path used by subsequent operations.
	void DirectoryHelper::SetAssetPath()
	{
		mAssetPath->Assign(*mProjectPath);
		mAssetPath->Append("\\Assets\\");
	}

	/// @brief Returns the project path used by this service.
	/// @return Borrowed access to the project path.
	CORE_API Common::FTDS::String* GetProjectPath()
	{
		return DirectoryHelper::GetInstance()->GetProjectPath();
	}

	/// @brief Returns the asset path used by this core.
	/// @return Borrowed access to the asset path.
	CORE_API Common::FTDS::String* Core::GetAssetPath()
	{
		return DirectoryHelper::GetInstance()->GetAssetPath();
	}

	/// @brief Returns the chunk path used by this service.
	/// @return Borrowed access to the chunk path.
	CORE_API Common::FTDS::String* GetChunkPath()
	{
		return DirectoryHelper::GetInstance()->GetChunkPath();
	}

	/// @brief Returns the chunk is saved used by this service.
	/// @return Current value of the chunk is saved flag.
	CORE_API bool GetChunkIsSaved()
	{
		return DirectoryHelper::GetInstance()->GetCurrChunkSaved();
	}

	/// @brief Updates the chunk is saved used by subsequent operations.
	/// @param val Replacement chunk is saved.
	CORE_API void SetChunkIsSaved(bool val)
	{
		return DirectoryHelper::GetInstance()->SetCurrChunkSaved(val);
	}
} // namespace Core
