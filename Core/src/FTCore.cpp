// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Plugin/Plugin.h"

#include <fstream>

#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileTypes.h"
#include "Manager/SceneManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/EventManager.h"
#include "Manager/DirectoryHelper.h"
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
	namespace GameData
	{
		constexpr const char* TITLE		 = "Game Data";
		constexpr const char* CHUNK_LIST = "Chunk List";
	} // namespace GameData

	class FTCore :
		public Core::Plugin
	{
	public:
		virtual void SaveProperties() {};
		virtual void LoadProperties() {};
		virtual void SaveManagerData(std::ofstream& ofs) {};
		virtual void LoadManagerData(std::ifstream& ifs) {};

	public:
		// FTDS::HashMap<Entity*>* GetEntities() { return mEntities; }
		// FTDS::HashMap<Plugin*>* GetPlugins() { return mPlugins; }
		Plugin* GetPlugin(FTDS::String& pluginName);

	public:
		void Initialize() override;

		void ProcessInput() override;
		void Render() override;
		void ProcessEvent() override;
		void ShutDown() override;

	public:
		FTCore(const char* name);
		~FTCore();

	private:
		Core::FTDS::String* mGameDataPath;

	private:
		void LoadGameData();
		void InitEntities();
	};

	void FTCore::LoadGameData()
	{
		std::ifstream ifs(mGameDataPath->C_Str());
		Core::FileIOHelper::BeginDataPackLoad(ifs, GameData::TITLE);

		std::pair<size_t, FTDS::String> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::CHUNK_LIST);
		for (size_t i = 0; i < chunkListPack.first; ++i)
		{
			FTDS::String* chunkTitle = DBG_NEW FTDS::String;
			FileIOHelper::LoadBasicString(ifs, *chunkTitle);
			SceneManager::GetInstance()->ChunkList()->PushBack(chunkTitle);
		}
		DirectoryHelper::GetInstance()->SetProjectPath(std::filesystem::absolute("./").string().c_str());
	}

	void FTCore::Initialize()
	{
		LoadGameData();
		InitEntities();
	}

	void FTCore::ProcessInput()
	{
	}

	void FTCore::Render()
	{
	}

	void FTCore::InitEntities()
	{
		Timer::GetInstance();
		SceneManager::GetInstance();
		ResourceManager::GetInstance();
		EventManager::GetInstance();
		DirectoryHelper::GetInstance();
		ChunkLoader::GetInstance();
	}

	void FTCore::ProcessEvent()
	{
		SceneManager::GetInstance()->ProcessEvent();
		EventManager::GetInstance()->ProcessEvent();
	}

	FTCore::FTCore(const char* name)
		: Core::Plugin(name)
		, mGameDataPath(
			  DBG_NEW FTDS::String("./"))
	{
		mGameDataPath->Append(GameData::TITLE);
		mGameDataPath->Append(FileTypes::GDPACK);
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

		PostQuitMessage(0);
	}

	extern "C"
	{
		extern "C" __declspec(dllexport) Plugin* CreatePlugin(const char* name)
		{
			return DBG_NEW Core::FTCore(name);
		}
	}
} // namespace Core