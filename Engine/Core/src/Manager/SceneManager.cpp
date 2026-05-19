// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/SceneManager.h"

#include <fstream>
#include <string>
#include <vector>

#include "Scene/Scene.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "FTDS/Static/FTString.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	SceneManager::SceneManager()
		: mChunkList(DBG_NEW Common::FTDS::DynamicArray<Common::FTDS::String*>())
		, mCurrentScene(nullptr)
	{
	}

	SceneManager::~SceneManager()
	{
		mChunkList->Clear();

		delete mChunkList;
		delete mCurrentScene;
	}

	void SceneManager::SwitchScene(size_t index)
	{
		mCurrentScene->DeleteAll();
		Common::FTDS::String path = Common::FTDS::String(".\\Chunks\\") + mChunkList->At(index)->C_Str();
		ChunkLoader::GetInstance()->LoadChunk(path.C_Str());
	}

	Scene* SceneManager::GetCurrentScene()
	{
		return mCurrentScene;
	}

	Common::FTDS::DynamicArray<Common::FTDS::String*>*& SceneManager::ChunkList()
	{
		return mChunkList;
	}

	void SceneManager::SetChunkListPath(Common::FTDS::String&& path)
	{
		mChunkListPath.Assign(path);
	}

	void SceneManager::Initialize(Scene* scene)
	{
		mCurrentScene = scene;
		SwitchScene(0);
	}

	void SceneManager::ProcessEvent()
	{
		mCurrentScene->ProcessEvent();
	}

	void SceneManager::SaveSceneList(std::ofstream& ofs)
	{
	}
} // namespace Core