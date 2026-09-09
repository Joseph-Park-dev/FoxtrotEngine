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
	/// @brief Initializes scene selection and chunk-list state.
	/// @note Initializes the :SceneManager base or delegates to its constructor.
	SceneManager::SceneManager()
		: mChunkList(DBG_NEW Common::FTDS::DynamicArray<Common::FTDS::String*>())
		, mCurrentScene(nullptr)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	SceneManager::~SceneManager()
	{
		for (auto it = mChunkList->Begin(); it != mChunkList->End(); ++it) delete *it;
		mChunkList->Clear();

		delete mChunkList;
		delete mCurrentScene;
	}

	/// @brief Selects and loads a scene from the configured chunk list.
	/// @param index Zero-based element index.
	void SceneManager::SwitchScene(size_t index)
	{
		mCurrentScene->DeleteAll();
		Common::FTDS::String path = Common::FTDS::String(".\\Chunks\\") + mChunkList->At(index)->C_Str();
		ChunkLoader::GetInstance()->LoadChunk(path.C_Str());
	}

	/// @brief Returns the current scene used by this scene manager.
	/// @return Borrowed access to the current scene.
	Scene* SceneManager::GetCurrentScene()
	{
		return mCurrentScene;
	}

	/// @brief Exposes the scene manager's ordered chunk-path collection.
	/// @return Borrowed access to the chunk list.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::FTDS::DynamicArray<Common::FTDS::String*>*& SceneManager::ChunkList()
	{
		return mChunkList;
	}

	/// @brief Updates the chunk list path used by subsequent operations.
	/// @param path Replacement chunk list path.
	void SceneManager::SetChunkListPath(Common::FTDS::String&& path)
	{
		mChunkListPath.Assign(path);
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param scene Scene managed or processed by the operation.
	void SceneManager::Initialize(Scene* scene)
	{
		mCurrentScene = scene;
		if (mChunkList && !mChunkList->IsEmpty()) SwitchScene(0);
	}

	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void SceneManager::ProcessEvent()
	{
		mCurrentScene->ProcessEvent();
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param ofs Output stream receiving the serialized data.
	void SceneManager::SaveSceneList(std::ofstream& ofs)
	{
	}

	/// @brief Searches the scene for an actor matching the supplied identifier or name.
	/// @param name Name used to identify the requested object or interface.
	/// @param filter Actor to exclude from the name search.
	/// @return Borrowed matching entry, or nullptr when the search does not find one.
	CORE_API Common::IActor* FindActor(const char* name, Common::IActor* filter)
	{
		return SceneManager::GetInstance()->GetCurrentScene()->FindActor(name, filter);
	}
} // namespace Core

namespace Core
{
    SceneManager* SceneManager::mInstance = nullptr;

    /// @brief Returns the shared instance used by this manager.
    /// @return Borrowed singleton pointer; do not delete it directly.
    /// @note Synchronize concurrent initialization, access, and destruction externally.
    SceneManager* SceneManager::GetInstance()
    {
        if (!mInstance)
            mInstance = DBG_NEW SceneManager();
        return mInstance;
    }

    /// @brief Deletes the shared instance and clears the singleton pointer.
    /// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
    void SceneManager::Destroy()
    {
        delete mInstance;
        mInstance = nullptr;
    }
}
