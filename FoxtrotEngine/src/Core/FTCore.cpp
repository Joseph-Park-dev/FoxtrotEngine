// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Core/FTCore.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Core/TemplateFunctions.h"
#include "Core/WindowProcess.h"
#include "Core/Timer.h"
#include "Actors/Actor.h"
#include "Components/SpriteRenderer.h"
#include "Managers/KeyInputManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/UIManager.h"
#include "Managers/CollisionManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Physics/Physics2D.h"
#include "Physics/ParticleSystem.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Scenes/Scene.h"

#include "Debugging/DebugMemAlloc.h"

// FTCore related singleton initializations -> used in the runtimes of the produced games.
Physics2D*			Physics2D::mInstance = nullptr;
Camera*				Camera::mInstance = nullptr;
ResourceManager*	ResourceManager::mInstance = nullptr;
CollisionManager*	CollisionManager::mInstance = nullptr;
SceneManager*		SceneManager::mInstance = nullptr;
UIManager*			UIManager::mInstance = nullptr;
EventManager*		EventManager::mInstance = nullptr;
KeyInputManager*	KeyInputManager::mInstance = nullptr;
ChunkLoader*		ChunkLoader::mInstance = nullptr;
ParticleSystem*		ParticleSystem::mInstance = nullptr;
Timer*				Timer::mInstance = nullptr;
FTCore*				FTCore::mInstance = nullptr;

void FTCore::LoadGameData()
{
	std::ifstream ifs(mGameDataPath);
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::GAME_DATA);
	std::pair<size_t, std::string> chunkListPack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::CHUNK_LIST);
	for (size_t i = 0; i < chunkListPack.first; ++i)
	{
		std::string chunkTitle = {};
		FileIOHelper::LoadBasicString(ifs, chunkTitle);
		SceneManager::GetInstance()->GetChunkList().push_back(chunkTitle);
	}
}

bool FTCore::Initialize()
{
	if (!InitializeWindow())
		return false;
	if (!InitFoxtrotRenderer_D3D11())
		return false;
	LoadGameData();
	InitSingletonManagers();
	InitTimer();
	return true;
}

bool FTCore::InitializeWindow()
{
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		mWindowTitle.c_str(), // lpszClassName, L-string
		NULL
	};
	if (!RegisterClassEx(&wc))
	{
		LogString("RegisterClassEx() failed.");
		return false;
	}
	RECT wr = { 0, 0, mWindowWidth, mWindowHeight };

	// 필요한 윈도우 크기(해상도) 계산
	// wr의 값이 바뀜
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	mWindow = CreateWindow(
		wc.lpszClassName,
		mWindowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_SYSMENU,
		100,                // x-coordinate, top left
		100,                // y-coordinate, top left
		wr.right - wr.left, // horizontal resolution
		wr.bottom - wr.top, // vertical resolution
		NULL, NULL, wc.hInstance, NULL);

	if (!mWindow)
	{
		LogString("CreateWindow() failed.");
		return false;
	}

	ShowWindow(mWindow, SW_SHOWDEFAULT);
	UpdateWindow(mWindow);
	return true;
}

bool FTCore::InitFoxtrotRenderer_D3D11()
{
	mGameRenderer = FoxtrotRenderer::CreateRenderer(mWindow, mWindowWidth, mWindowHeight);
	if (!mGameRenderer)
		return false;
	return true;
}

void FTCore::InitSingletonManagers()
{
	Physics2D::GetInstance()->Initialize();
	Camera::GetInstance()->Initialize(mGameRenderer, 64.f, 1.8f);
	ResourceManager::GetInstance()->Initialize(mGameRenderer);
	CollisionManager::GetInstance()->Initialize();
	SceneManager::GetInstance()->Initialize();
	UIManager::GetInstance();
	EventManager::GetInstance();
	KeyInputManager::GetInstance();
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
	MSG msg = {};
	KeyInputManager::GetInstance()->DetectKeyInput();
	KeyInputManager::GetInstance()->DetectMouseInput(msg);
	//KeyInputManager::GetInstance()->DetectGamepadInput();
	SceneManager::GetInstance()->ProcessInput(KeyInputManager::GetInstance());
	TranslateMessage(&msg);
	DispatchMessage(&msg);
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
	UIManager::GetInstance()->Update(deltaTime);
	Camera::GetInstance()->Update(deltaTime);
}

void FTCore::GenerateOutput()
{
	mGameRenderer->RenderClear();
	MSG msg = {};
	InvalidateRect(mWindow, NULL, true);
	if (PeekMessage(&msg, mWindow, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UpdateWindow(mWindow);

	SceneManager::GetInstance()->Render(mGameRenderer);
	ParticleSystem::GetInstance()->Render(mGameRenderer);
	mGameRenderer->SwapChainPresent(1, 0);
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
	, mWindowWidth(1920)
	, mWindowHeight(1080)
	, mWindowTitle(L"Untitled Game Project")
	, mGameDataPath(
		std::string("./")
		+ std::string(ChunkKeys::GAME_DATA)
		+ std::string(ChunkKeys::GAMEDATA_FILE_FORMAT)
		)
{}

FTCore::~FTCore()
{}

void FTCore::ShutDown()
{
	SceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
	ResourceManager::GetInstance()->DeleteAll();
	Physics2D::GetInstance()->ShutDown();
	FoxtrotRenderer::DestroyRenderer(mGameRenderer);
	CollisionManager::GetInstance()->Destroy();

	EventManager::GetInstance()->Destroy();
	KeyInputManager::GetInstance()->Destroy();
	ResourceManager::GetInstance()->Destroy();
	SceneManager::GetInstance()->Destroy();
	UIManager::GetInstance()->Destroy();
	Physics2D::GetInstance()->Destroy();
	ChunkLoader::GetInstance()->Destroy();
	Camera::GetInstance()->Destroy();
	Timer::GetInstance()->Destroy();
	ParticleSystem::GetInstance()->Destroy();

	DestroyWindow(mWindow);
	PostQuitMessage(0);
}

LRESULT FTCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
	{
		SetIsRunning(false);
		return 0;
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}