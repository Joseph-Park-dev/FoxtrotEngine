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
#include "Actor/IActor.h"

namespace Core
{
	SceneManager::SceneManager()
		: mChunkList(DBG_NEW Common::FTDS::DynamicArray<Common::FTDS::String*>())
		, mCurrentScene(nullptr)
	{
	}

	SceneManager::~SceneManager()
	{
		for (auto it = mChunkList->Begin(); it != mChunkList->End(); ++it) delete *it;
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
		if (mChunkList && !mChunkList->IsEmpty()) SwitchScene(0);
	}

	void SceneManager::ProcessEvent()
	{
		mCurrentScene->ProcessEvent();
	}

	void SceneManager::SaveSceneList(std::ofstream& ofs)
	{
	}

	CORE_API Common::IActor* FindActor(const char* name, Common::IActor* filter)
	{
		return SceneManager::GetInstance()->GetCurrentScene()->FindActor(name, filter);
	}
} // namespace Core

namespace Core
{
    SceneManager* SceneManager::mInstance = nullptr;

    SceneManager* SceneManager::GetInstance()
    {
        if (!mInstance)
            mInstance = DBG_NEW SceneManager();
        return mInstance;
    }

    void SceneManager::Destroy()
    {
        delete mInstance;
        mInstance = nullptr;
    }
}
