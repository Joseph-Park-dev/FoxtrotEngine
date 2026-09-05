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

	bool FTCore::Initialize()
	{
		LoadGameData();

		PluginManager* plugins = PluginManager::GetInstance();
		if (!plugins->RegisterPlugin(Core::Plugin::Name::D3D11))
			return false;

#ifdef FOXTROT_EDITOR
		if (!plugins->RegisterPlugin(Core::Plugin::Name::EDITOR))
			return false;
#endif
		Setup();
		return true;
	}

	void FTCore::Setup()
	{
	}

	void FTCore::RunLoop()
	{
		while (mIsRunning)
		{
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

	void FTCore::ProcessInput()
	{
		PluginManager::GetInstance()->ProcessInput();
	}

	void FTCore::Update()
	{
		Core::Timer::GetInstance()->Update();
		float deltaTime = Core::Timer::GetInstance()->GetDeltaTime();

		if (!mIsUpdating)
			return;
		PluginManager::GetInstance()->Update(deltaTime);
	}

	void FTCore::LateUpdate()
	{
		if (!mIsUpdating)
			return;

		float deltaTime = Core::Timer::GetInstance()->GetDeltaTime();
		PluginManager::GetInstance()->LateUpdate(deltaTime);
	}

	void FTCore::Render()
	{
		PluginManager::GetInstance()->Render();
	}

	void FTCore::ProcessEvent()
	{
		SceneManager::GetInstance()->ProcessEvent();
		EventManager::GetInstance()->ProcessEvent();
	}

	FTCore::FTCore()
		: mGameDataPath(DBG_NEW Common::FTDS::String("./"))
		, mIsRunning(true)
		, mIsUpdating(true)
	{
		mGameDataPath->Append(GameData::TITLE);
		mGameDataPath->Append(Common::FileTypes::GDPACK);
	}

	FTCore::~FTCore()
	{
		delete mGameDataPath;
	}

	void FTCore::ShutDown()
	{
		SceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
		SceneManager::GetInstance()->Destroy();
		ResourceManager::GetInstance()->Destroy();
		EventManager::GetInstance()->Destroy();
		ChunkLoader::GetInstance()->Destroy();
		Timer::GetInstance()->Destroy();

		PluginManager::GetInstance()->ShutDown();
		PluginManager::GetInstance()->Destroy();
		PostQuitMessage(0);
	}
} // namespace Core

extern "C"
{
	CORE_API Core::FTCore* GetCore()
	{
		return Core::FTCore::GetInstance();
	}

	CORE_API void DestroyCore()
	{
		Core::FTCore::Destroy();
	}
}