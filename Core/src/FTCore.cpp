// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTCore.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileTypes.h"
#include "Manager/SceneManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/EventManager.h"
#include "Manager/DirectoryHelper.h"
#include "Plugin/Plugin.h"
#include "TemplateFunctions.h"
#include "Timer.h"
#include "Renderer/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/FoxtrotRenderer.h"
#include "InputSystem/FTInputDevice.h"
#include "Scene/Scene.h"
#include "Static/HashMap.h"
#include "Static/FTString.h"

namespace Core
{
	using PLUGIN_CONSTRUCT = Plugin* (*)(FTCore * base);

	FTCore* FTCore::mInstance = nullptr;
	// Timer*			 Timer::mInstance			= nullptr;
	// SceneManager*	 SceneManager::mInstance	= nullptr;
	// ResourceManager* ResourceManager::mInstance = nullptr;
	// EventManager*	 EventManager::mInstance	= nullptr;
	// DirectoryHelper* DirectoryHelper::mInstance = nullptr;
	// ChunkLoader*	 ChunkLoader::mInstance		= nullptr;

	void FTCore::LoadGameData()
	{
		std::ifstream ifs(mGameDataPath->C_Str());
		FileIOHelper::BeginDataPackLoad(ifs, GameData::TITLE);

		std::pair<size_t, FTDS::String> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::CHUNK_LIST);
		for (size_t i = 0; i < chunkListPack.first; ++i)
		{
			FTDS::String* chunkTitle = DBG_NEW FTDS::String;
			FileIOHelper::LoadBasicString(ifs, *chunkTitle);
			SceneManager::GetInstance()->ChunkList()->PushBack(chunkTitle);
		}
		DirectoryHelper::GetInstance()->SetProjectPath(std::filesystem::absolute("./").string().c_str());
	}

	bool FTCore::Initialize()
	{
		LoadGameData();

		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Initialize();

		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Setup();

		InitTimer();
		return true;
	}

	void FTCore::LoadDLL(FTDS::String& path)
	{
		FTDS::String name;
		ExtractFileName(path, name);
	}

	void FTCore::InitTimer()
	{
		Timer::GetInstance();
	}

	void FTCore::InitEntities()
	{
		Timer::Initialize(this);
		SceneManager::Initialize(this);
		ResourceManager::Initialize(this);
		EventManager::Initialize(this);
		DirectoryHelper::Initialize(this);
		ChunkLoader::Initialize(this);

		mEntities->Insert("Timer", Timer::GetInstance());
		mEntities->Insert("SceneManager", SceneManager::GetInstance());
		mEntities->Insert("CoreResourceManager", ResourceManager::GetInstance());
		mEntities->Insert("EventManager", EventManager::GetInstance());
		mEntities->Insert("DirectoryHelper", DirectoryHelper::GetInstance());
		mEntities->Insert("ChunkLoader", ChunkLoader::GetInstance());
	}

	void FTCore::RunLoop()
	{
		while (mIsRunning)
		{
			ProcessInput();
			UpdateGame();
			GenerateOutput();

			ProcessEvent();
		}
	}

	Plugin* FTCore::RegisterPlugin(HMODULE mod, FTDS::String& pluginName)
	{
		FARPROC			 proc	  = GetProcAddress(mod, PluginKey::CREATE_PLUGIN);
		PLUGIN_CONSTRUCT plgConst = (PLUGIN_CONSTRUCT)GetProcAddress(mod, PluginKey::CREATE_PLUGIN);
		Plugin*			 plugin	  = plgConst(this);
		plugin->SetModule(mod);
		mPlugins->Insert(pluginName, plugin);

		return plugin;
	}

	void FTCore::ProcessInput()
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->ProcessInput();
	}

	void FTCore::UpdateGame()
	{
		Timer::GetInstance()->Update();
		float deltaTime = Timer::GetInstance()->GetDeltaTime();

		if (!mIsUpdating)
			return;

		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Update(deltaTime);

		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->LateUpdate(deltaTime);
	}

	void FTCore::GenerateOutput()
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Render();
	}

	void FTCore::ProcessEvent()
	{
		SceneManager::GetInstance()->ProcessEvent();
		EventManager::GetInstance()->ProcessEvent();
	}

	FTCore::FTCore()
		: mModule()
		, mIsRunning(true)
		, mIsUpdating(true)
		, mGameDataPath(
			  DBG_NEW FTDS::String("./"))
		, mPlugins(DBG_NEW FTDS::HashMap<Plugin*>())
		, mEntities(DBG_NEW FTDS::HashMap<Entity*>)
	{
		mGameDataPath->Append(ChunkKey::GAME_DATA);
		mGameDataPath->Append(FileTypes::GDPACK);
	}

	FTCore::~FTCore()
	{
		delete mGameDataPath;
		delete mPlugins;
	}

	void FTCore::ShutDown()
	{
		Safe_Delete_Map(mPlugins);
		Safe_Delete_Map(mEntities);

		SceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
		SceneManager::GetInstance()->Destroy();
		ResourceManager::GetInstance()->Destroy();
		EventManager::GetInstance()->Destroy();
		ChunkLoader::GetInstance()->Destroy();
		Timer::GetInstance()->Destroy();

		PostQuitMessage(0);
	}

	Plugin* FTCore::GetPlugin(FTDS::String& pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

	extern "C"
	{

		FTCore* GetInstanceCore()
		{
			return FTCore::GetInstance();
		}

		void DestroyCore()
		{
			FTCore::GetInstance()->Destroy();
		}
	}
} // namespace Core