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
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
#include "Plugin/Plugin.h"
#include "TemplateFunctions.h"

FTCore* FTCore::mInstance = nullptr;

void FTCore::LoadGameData()
{
	std::ifstream ifs(mGameDataPath.C_Str());
	FileIOHelper::BeginDataPackLoad(ifs, GameData::TITLE);

	std::pair<size_t, FTDS::String> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::CHUNK_LIST);
	for (size_t i = 0; i < chunkListPack.first; ++i)
	{
		FTDS::String chunkTitle = {};
		FileIOHelper::LoadBasicString(ifs, chunkTitle);
		SceneManager::GetInstance()->GetChunkList().push_back(chunkTitle);
	}

	std::pair<size_t, FTDS::String> dllPack = FileIOHelper::BeginDataPackLoad(ifs, GameData::DLL_LIST);
	mPlugins->Reserve(dllPack.first);
	for (size_t i = 0; i < dllPack.first; ++i)
	{
		FTDS::String chunkTitle = {};
		FileIOHelper::LoadBasicString(ifs, chunkTitle);
		SceneManager::GetInstance()->GetChunkList().push_back(chunkTitle);
	}



	std::filesystem::path assetPath = std::filesystem::absolute("./");
	ResourceManager::GetInstance()->SetPathToAsset(assetPath.string().c_str());
}

bool FTCore::Initialize()
{
	LoadGameData();

	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Initialize();

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
	FTResourceDef dllDef(path);
	mPluginMap
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
	mWindow->ProcessInput();
	SceneManager::GetInstance()->ProcessInput(mWindow->GetInputDevice());
}

void FTCore::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	SceneManager::GetInstance()->Update(deltaTime);
	SceneManager::GetInstance()->Lateupdate(deltaTime);
	SoundManager::GetInstance()->Update();
	Physics2D::GetInstance()->Update();
	CollisionManager::GetInstance()->Update();
	ParticleSystem::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime, mWindow->GetInputDevice());
	Camera::GetInstance()->Update(deltaTime);
}

void FTCore::GenerateOutput()
{
	// mGameRenderer->RenderClear(mWindow);
	mWindow->BeginRender(mGameRenderer);

	FTVector2 size = GetGameWindow()->GetRenderArea()->GetSize();
	mGameRenderer->SetViewport(0, 0, size.x, size.y);

	if (!ChunkLoader::GetInstance()->IsLoadingChunk())
	{
		SceneManager::GetInstance()->Render(mGameRenderer);
		ParticleSystem::GetInstance()->Render(mGameRenderer);
		DebugShapes::GetInstance()->Render(mGameRenderer);
		LightManager::GetInstance()->Render(mGameRenderer, Camera::GetInstance());
		mWindow->SamplCursorPosColor(mGameRenderer->GetContext(), mGameRenderer->GetCursorPosColor());
	}

	mWindow->GetSwapChain()->Present(1, 0);
}

void FTCore::ProcessEvent()
{
	SceneManager::GetInstance()->ProcessEvent();
	EventManager::GetInstance()->ProcessEvent();
}

FTCore::FTCore()
	: mWindow(nullptr)
	, mGameRenderer(nullptr)
	, mIsRunning(true)
	, mGameDataPath(
		  FTDS::String("./") + FTDS::String(ChunkKey::GAME_DATA) + FTDS::String(FileTypes::GDPACK))
{
	mPluginMap
}

FTCore::~FTCore()
{
	delete mWindow;
}

void FTCore::ShutDown()
{
	DebugShapes::GetInstance()->DeleteAll();
	SceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
	Physics2D::GetInstance()->ShutDown();
	FoxtrotRenderer::DestroyRenderer(mGameRenderer);

	SceneManager::GetInstance()->Destroy();
	ResourceManager::GetInstance()->Destroy();
	CollisionManager::GetInstance()->Destroy();
	DebugShapes::GetInstance()->Destroy();
	SoundManager::GetInstance()->Destroy();
	EventManager::GetInstance()->Destroy();
	AnimationManager::GetInstance()->Destroy();
	delete gSpineExtension;
	gSpineExtension = nullptr;
	UIManager::GetInstance()->Destroy();
	Physics2D::GetInstance()->Destroy();
	ChunkLoader::GetInstance()->Destroy();
	Camera::GetInstance()->Destroy();
	Timer::GetInstance()->Destroy();
	ParticleSystem::GetInstance()->Destroy();
	LightManager::GetInstance()->Destroy();

	PostQuitMessage(0);
}

LRESULT FTCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			SetIsRunning(false);
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

extern "C"
{
	FTCore* Create_Core()
	{
		return new FTCore();
	}
}