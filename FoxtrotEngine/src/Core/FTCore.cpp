// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Core/FTCore.h"

#include <windows.h>
#define WIN32_LEAN_AND_MEAN

#include "Core/TemplateFunctions.h"
#include "Core/WindowProcess.h"
#include "Core/Timer.h"
#include "Actors/Actor.h"
#include "Components/SpriteRenderer.h"
#include "InputSystem/FTInputDevice.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/UIManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/LightManager.h"
#include "Managers/DebugShapes.h"
#include "Managers/AnimationManager.h"
#include "Managers/TileMapManager.h"
#include "WindowSystem/FTWindow.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Renderer/FTRectArea.h"
#include "Physics/Physics2D.h"
#include "Physics/ParticleSystem.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Scenes/Scene.h"
#include "Debugging/DebugMemAlloc.h"

#include "Static/FTString.h"

// FTCore related singleton initializations -> used in the runtimes of the produced games.
Physics2D*		  Physics2D::mInstance		  = nullptr;
Camera*			  Camera::mInstance			  = nullptr;
AnimationManager* AnimationManager::mInstance = nullptr;
TileMapManager*	  TileMapManager::mInstance	  = nullptr;
ResourceManager*  ResourceManager::mInstance  = nullptr;
CollisionManager* CollisionManager::mInstance = nullptr;
SceneManager*	  SceneManager::mInstance	  = nullptr;
UIManager*		  UIManager::mInstance		  = nullptr;
EventManager*	  EventManager::mInstance	  = nullptr;
ChunkLoader*	  ChunkLoader::mInstance	  = nullptr;
ParticleSystem*	  ParticleSystem::mInstance	  = nullptr;
LightManager*	  LightManager::mInstance	  = nullptr;
Timer*			  Timer::mInstance			  = nullptr;
DebugShapes*	  DebugShapes::mInstance	  = nullptr;
FTCore*			  FTCore::mInstance			  = nullptr;

void FTCore::LoadGameData()
{
	std::ifstream ifs(mGameDataPath.C_Str());
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::GAME_DATA);
	std::pair<size_t, FTDS::String> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::CHUNK_LIST);
	for (size_t i = 0; i < chunkListPack.first; ++i)
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
	if (mWindow)
	{
		delete mWindow;
		mWindow = nullptr;
	}
	mWindow = DBG_NEW FTWindow(mWindowTitle.C_Str(), mWindowWidth, mWindowHeight);
	mWindow->GetRenderArea()->Set(0.f, 0.f, mWindowWidth, mWindowHeight);
	if (!mWindow->InitializeWindow(WndProc))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow");
		return false;
	}

	if (!InitFoxtrotRenderer_D3D11(mWindow, mWindowWidth, mWindowHeight))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTRenderer");
		return false;
	}

	if (!mWindow->InitializeWindowRenderer(GetGameRenderer()))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow Renderer");
		return false;
	}

	LoadGameData();
	InitSingletonManagers();
	InitTimer();
	return true;
}

bool FTCore::InitFoxtrotRenderer_D3D11(FTWindow* window, int& width, int& height)
{
	mGameRenderer = FoxtrotRenderer::CreateRenderer(window, width, height);
	if (!mGameRenderer)
		return false;
	return true;
}

void FTCore::InitSingletonManagers()
{
	Physics2D::GetInstance()->Initialize();
	Camera::GetInstance()->Initialize(mWindow, 64, 1.8f);
	DebugShapes::GetInstance()->Initialize(mGameRenderer);
	CollisionManager::GetInstance()->Initialize();
	ResourceManager::GetInstance()->Initialize(mGameRenderer);
	UIManager::GetInstance();
	EventManager::GetInstance();
	LightManager::GetInstance()->Initialize(mGameRenderer);
	AnimationManager::GetInstance()->Initialize(mGameRenderer);
	TileMapManager::GetInstance();
	SceneManager::GetInstance()->Initialize();
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
	, mWindowWidth(1280)
	, mWindowHeight(720)
	, mWindowTitle("Foxtrot Engine Showcase (ver.0.1.2)")
	, mGameDataPath(
		  FTDS::String("./") + FTDS::String(ChunkKey::GAME_DATA) + FTDS::String(FileTypes::GDPACK))
{
}

FTCore::~FTCore()
{
	delete mWindow;
}

void FTCore::ShutDown()
{
	SceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
	ResourceManager::GetInstance()->DeleteAll();
	Physics2D::GetInstance()->ShutDown();
	FoxtrotRenderer::DestroyRenderer(mGameRenderer);
	CollisionManager::GetInstance()->Destroy();

	
	AnimationManager::GetInstance()->Destroy();
	delete gSpineExtension;
	gSpineExtension = nullptr;
	EventManager::GetInstance()->Destroy();
	ResourceManager::GetInstance()->Destroy();
	SceneManager::GetInstance()->Destroy();
	UIManager::GetInstance()->Destroy();
	Physics2D::GetInstance()->Destroy();
	ChunkLoader::GetInstance()->Destroy();
	Camera::GetInstance()->Destroy();
	Timer::GetInstance()->Destroy();
	ParticleSystem::GetInstance()->Destroy();
	LightManager::GetInstance()->Destroy();
	TileMapManager::GetInstance()->Destroy();

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