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
#include "Plugin/Plugin.h"
#include "TemplateFunctions.h"
#include "Timer.h"
#include "Renderer/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "InputSystem/FTInputDevice.h"
#include "Scene/Scene.h"
#include "Static/HashMap.h"
#include "Static/FTString.h"

FTCore*			 FTCore::mInstance			= nullptr;
SceneManager*	 SceneManager::mInstance	= nullptr;
ResourceManager* ResourceManager::mInstance = nullptr;
ChunkLoader*	 ChunkLoader::mInstance		= nullptr;
Camera*			 Camera::mInstance			= nullptr;
Timer*			 Timer::mInstance			= nullptr;
EventManager*	 EventManager::mInstance	= nullptr;

void FTCore::LoadGameData()
{
	std::ifstream ifs(mGameDataPath->C_Str());
	FileIOHelper::BeginDataPackLoad(ifs, GameData::TITLE);

	std::pair<size_t, FTDS::String> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::CHUNK_LIST);
	for (size_t i = 0; i < chunkListPack.first; ++i)
	{
		FTDS::String chunkTitle = {};
		FileIOHelper::LoadBasicString(ifs, chunkTitle);
		SceneManager::GetInstance()->ChunkList()->PushBack(chunkTitle);
	}

	std::pair<size_t, FTDS::String> dllPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::DLL_LIST);
	mLoadedPlugins->Reserve(dllPack.first);
	for (size_t i = 0; i < dllPack.first; ++i)
	{
		FTDS::String fileName	  = {};
		FTDS::String relativePath = {};
		FileIOHelper::LoadBasicString(ifs, fileName);
		FileIOHelper::LoadBasicString(ifs, relativePath);

		FTResourceDef def(fileName, relativePath);
		Plugin* plugin = DBG_NEW Plugin(def);
		mLoadedPlugins->PushBack(plugin);
	}

	std::filesystem::path assetPath = std::filesystem::absolute("./");
	ResourceManager::GetInstance()->SetPathToAsset(assetPath.string().c_str());
}

bool FTCore::Initialize()
{
	LoadGameData();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Initialize();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Setup();

	InitSingletonManagers();
	InitTimer();
	return true;
}

void FTCore::InitSingletonManagers()
{
	ResourceManager::GetInstance()->Initialize(mGameRenderer);
	SceneManager::GetInstance()->Initialize();
}

void FTCore::LoadDLL(FTDS::String& path)
{
	FTDS::String name;
	ExtractFileName(path, name);
	FTResourceDef dllDef(name, path);
}

void FTCore::InitTimer()
{
	Timer::GetInstance();
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

void FTCore::SetWindow(FTWindow* window)
{
	mWindow = window;
}

void FTCore::SetInputDevice(FTInputDevice* device)
{
	mInputDevice = device;
}

void FTCore::SetRenderer(FoxtrotRenderer* renderer)
{
	mGameRenderer = renderer;
}

void FTCore::ProcessInput()
{
	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->ProcessInput(mInputDevice);
}

void FTCore::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Update(deltaTime);

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->LateUpdate(deltaTime);
}

void FTCore::GenerateOutput()
{
	// mGameRenderer->RenderClear(mWindow);
	mWindow->BeginRender(mGameRenderer);

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Render(mGameRenderer);

	mWindow->EndRender(mGameRenderer);
}

void FTCore::ProcessEvent()
{
	SceneManager::GetInstance()->ProcessEvent();
	EventManager::GetInstance()->ProcessEvent();
}

FTCore::FTCore()
	: mWindow(nullptr)
	, mInputDevice(nullptr)
	, mGameRenderer(nullptr)
	, mIsRunning(true)
	, mGameDataPath(
		  DBG_NEW FTDS::String("./"))
	, mLoadedPlugins(DBG_NEW FTDS::DynamicArray<Plugin*>())
{
	mGameDataPath->Append(ChunkKey::GAME_DATA);
	mGameDataPath->Append(FileTypes::GDPACK);
}

FTCore::~FTCore()
{
	delete mWindow;
	delete mInputDevice;
	delete mGameRenderer;
	delete mGameDataPath;
	delete mLoadedPlugins;
}

void FTCore::ShutDown()
{
	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
	{
		(*iter)->Clear();
		delete (*iter);
	}

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
	FTCore* GetInstanceCore()
	{
		return FTCore::GetInstance();
	}

	void DestroyCore()
	{
		FTCore::GetInstance()->Destroy();
	}
}