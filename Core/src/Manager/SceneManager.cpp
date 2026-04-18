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
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "Static/FTString.h"
#include "Dynamic/DynamicArray.h"

namespace Core
{
	SceneManager::SceneManager()
		: mChunkList(DBG_NEW FTDS::DynamicArray<FTDS::String*>())
		, mCurrentScene(nullptr)
	{
	}

	SceneManager::~SceneManager()
	{
		mChunkList->Clear();

		delete mChunkList;
		delete mCurrentScene;
	}

	void SceneManager::RegisterMemberFuncs()
	{
	}

	void SceneManager::SwitchScene(size_t index)
	{
		mCurrentScene->DeleteAll();
		FTDS::String path = FTDS::String(".\\Chunks\\") + mChunkList->At(index)->C_Str();
		ChunkLoader::GetInstance()->LoadChunk(path);
	}

	Scene* SceneManager::GetCurrentScene()
	{
		return mCurrentScene;
	}

	FTDS::DynamicArray<FTDS::String*>*& SceneManager::ChunkList()
	{
		return mChunkList;
	}

	void SceneManager::SetChunkListPath(FTDS::String&& path)
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