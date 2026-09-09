#include "Plugin/GameSystems.h"
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Core/FTCore.h"

#include <fstream>
#include <Windows.h>
#include "Foxtrot/Runtime/PlatformApi.h"

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileTypes.h"
#include "Manager/SceneManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/EventManager.h"
#include "Manager/DirectoryHelper.h"
#include "Manager/PluginManager.h"
#include "TemplateFunctions.h"
#include "Manager/Timer.h"
#include "Scene/Scene.h"
#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"

#include "Plugin/PluginKey.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	namespace GameData
	{
		constexpr const char* TITLE		 = "Game Data";
		constexpr const char* CHUNK_LIST = "Chunk List";
	} // namespace GameData

	/// @brief Reads the configured game data needed to initialize runtime content.
	void FTCore::LoadGameData()
	{
		std::ifstream ifs(mGameDataPath->C_Str());
		if (!ifs.good())
			return;

		Common::FileIOHelper::BeginDataPackLoad(ifs, GameData::TITLE);

		std::pair<size_t, Common::FTDS::String> chunkListPack = Common::FileIOHelper::BeginDataPackLoad(ifs, GameData::CHUNK_LIST);
		for (size_t i = 0; i < chunkListPack.first; ++i)
		{
			Common::FTDS::String* chunkTitle = DBG_NEW Common::FTDS::String;
			Common::FileIOHelper::LoadBasicString(ifs, *chunkTitle);
			SceneManager::GetInstance()->ChunkList()->PushBack(chunkTitle);
		}
		DirectoryHelper::GetInstance()->SetProjectPath(std::filesystem::absolute("./").string().c_str());
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool FTCore::Initialize()
	{
		LoadGameData();

        if (!SceneManager::GetInstance()->GetCurrentScene())
            SceneManager::GetInstance()->Initialize(new Scene);
		Setup();
		return true;
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	void FTCore::Setup()
	{
	}

	/// @brief Runs input, update, rendering, and event processing until the engine stops.
	void FTCore::RunLoop() { RunFrames(0); }

    /// @brief Runs the engine loop for a bounded number of frames.
    /// @param maxFrames Maximum number of frames to run.
    void FTCore::RunFrames(unsigned int maxFrames)
	{
        unsigned int frame = 0;
		while (mIsRunning && (!maxFrames || frame++ < maxFrames))
		{
			FtBeginInputFrame();
            MSG msg{};
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					mIsRunning = false;
					break;
				}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (!mIsRunning)
				break;

			ProcessInput();
			Update();
			LateUpdate();
			Render();
			ProcessEvent();
		}
	}

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	void FTCore::ProcessInput()
	{
		PluginManager::GetInstance()->ProcessInput();
	}

	/// @brief Advances frame-dependent state using the current time step.
	void FTCore::Update()
	{
		Core::Timer::GetInstance()->Update();
		float deltaTime = Core::Timer::GetInstance()->GetDeltaTime();

		if (!mIsUpdating)
			return;
		Foxtrot::UpdateGameSystems(deltaTime);
		PluginManager::GetInstance()->Update(deltaTime);
	}

	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	void FTCore::LateUpdate()
	{
		if (!mIsUpdating)
			return;

		float deltaTime = Core::Timer::GetInstance()->GetDeltaTime();
		PluginManager::GetInstance()->LateUpdate(deltaTime);
	}

	/// @brief Submits this object's graphics work for the current frame.
	void FTCore::Render()
	{
		PluginManager::GetInstance()->Render();
	}

	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void FTCore::ProcessEvent()
	{
		SceneManager::GetInstance()->ProcessEvent();
		EventManager::GetInstance()->ProcessEvent();
	}

	/// @brief Initializes engine loop flags and runtime service state.
	/// @note Initializes the :FTCore base or delegates to its constructor.
	FTCore::FTCore()
		: mGameDataPath(DBG_NEW Common::FTDS::String("./"))
		, mIsRunning(true)
		, mIsUpdating(true)
	{
		mGameDataPath->Append(GameData::TITLE);
		mGameDataPath->Append(Common::FileTypes::GDPACK);
	}

	/// @brief Releases the resources managed by this instance during destruction.
	FTCore::~FTCore()
	{
		delete mGameDataPath;
	}

	/// @brief Releases runtime services and resources during engine shutdown.
	void FTCore::ShutDown()
	{
		if (auto scene = SceneManager::GetInstance()->GetCurrentScene()) scene->DeleteAll();
		SceneManager::GetInstance()->Destroy();
		ResourceManager::GetInstance()->Destroy();
		EventManager::GetInstance()->Destroy();
		ChunkLoader::GetInstance()->Destroy();
		Timer::Destroy();
		DirectoryHelper::Destroy();

		PluginManager::GetInstance()->ShutDown();
		PluginManager::GetInstance()->Destroy();
		FtDestroyAllNativeWindows();
	}
} // namespace Core

extern "C"
{
	/// @brief Returns the core used by this service.
	/// @return Borrowed access to the core.
	CORE_API Core::FTCore* GetCore()
	{
		return Core::FTCore::GetInstance();
	}

	/// @brief Shuts down and releases the core engine instance.
	CORE_API void DestroyCore()
	{
		Core::FTCore::Destroy();
	}
}
