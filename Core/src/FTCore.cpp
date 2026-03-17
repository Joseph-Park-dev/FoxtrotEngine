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
		FTDS::String chunkTitle = {};
		FileIOHelper::LoadBasicString(ifs, chunkTitle);
		SceneManager::GetInstance()->ChunkList()->PushBack(chunkTitle);
	}

	std::filesystem::path assetPath = std::filesystem::absolute("./");
	ResourceManager::GetInstance()->SetPathToAsset(assetPath.string().c_str());
}

bool FTCore::Initialize()
{
	LoadGameData();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->Initialize();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->Setup();

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

void FTCore::ProcessInput()
{
	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->ProcessInput(mInputDevice);
}

void FTCore::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->Update(deltaTime);

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->LateUpdate(deltaTime);
}

void FTCore::GenerateOutput()
{
	// mGameRenderer->RenderClear(mWindow);
	mWindow->BeginRender(mGameRenderer);

	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
		(*iter)->Value()->Render(mGameRenderer);

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
{
	mGameDataPath->Append(ChunkKey::GAME_DATA);
	mGameDataPath->Append(FileTypes::GDPACK);
	mLoadedPlugins = DBG_NEW FTDS::HashMap<Plugin*>();
}

FTCore::~FTCore()
{
	delete mWindow;
	delete mInputDevice;
	FoxtrotRenderer::Destroy(mGameRenderer);
	delete mLoadedPlugins;
}

void FTCore::ShutDown()
{
	for (auto iter = mLoadedPlugins->Begin(); iter != mLoadedPlugins->End(); ++iter)
	{
		(*iter)->Value()->Clear();
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
	void Create_Core()
	{
		FTCore::GetInstance();
	}
}