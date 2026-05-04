// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ICore.h"

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
#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"

namespace Core
{
	namespace GameData
	{
		constexpr const char* TITLE		 = "Game Data";
		constexpr const char* CHUNK_LIST = "Chunk List";
	} // namespace GameData

	class FTCore :
		public Core::IPlugin
	{
	public:
		Core::Entity* GetEntity(const char* name) override;

	public:
		//////////////////////////////////
		////// Initialization Phase //////
		//////////////////////////////////
		// clang-format off
		virtual void Initialize()	override;
		virtual void Setup()		override;
		// clang-format on

		///////////////////////
		////// Game Loop //////
		///////////////////////
		// clang-format off
		virtual void ProcessInput()				 override;
		virtual void Update(float deltaTime)	 override;
		virtual void LateUpdate(float deltaTime) override;
		virtual void Render()					 override;
		virtual void ProcessEvent()				 override;
		// clang-format on

		///////////////////////////////
		////// Termination Phase //////
		///////////////////////////////
		virtual void ShutDown() override;

	public:
		FTCore();
		~FTCore();

	private:
		Common::FTDS::String*				  mGameDataPath;
		Common::FTDS::HashMap<Core::Entity*>* mEntities;

	private:
		void LoadGameData();
		void InitEntities();
	};

	void FTCore::LoadGameData()
	{
		std::ifstream ifs(mGameDataPath->C_Str());
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

	Core::Entity* FTCore::GetEntity(const char* name)
	{
		return mEntities->At(name)->Value();
	}

	void FTCore::Initialize()
	{
		LoadGameData();
		InitEntities();
	}

	void FTCore::Setup()
	{
	}

	void FTCore::ProcessInput()
	{
	}

	void FTCore::Update(float deltaTime)
	{
	}

	void FTCore::LateUpdate(float deltaTime)
	{
	}

	void FTCore::Render()
	{
	}

	void FTCore::InitEntities()
	{
		mEntities->Insert("Timer", Timer::GetInstance());
		mEntities->Insert("SceneManager", SceneManager::GetInstance());
		mEntities->Insert("ResourceManager", ResourceManager::GetInstance());
		mEntities->Insert("EventManager", EventManager::GetInstance());
		mEntities->Insert("DirectoryHelper", DirectoryHelper::GetInstance());
		mEntities->Insert("ChunkLoader", ChunkLoader::GetInstance());
	}

	void FTCore::ProcessEvent()
	{
		SceneManager::GetInstance()->ProcessEvent();
		EventManager::GetInstance()->ProcessEvent();
	}

	FTCore::FTCore()
		: mEntities(DBG_NEW Common::FTDS::HashMap<Core::Entity*>)
		, mGameDataPath(
			  DBG_NEW Common::FTDS::String("./"))
	{
		mGameDataPath->Append(GameData::TITLE);
		mGameDataPath->Append(FileTypes::GDPACK);
	}

	FTCore::~FTCore()
	{
		Safe_Delete_Map(mEntities);
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
		extern "C" __declspec(dllexport) IPlugin* CreatePlugin(const char* name)
		{
			return DBG_NEW Core::FTCore();
		}
	}
} // namespace Core