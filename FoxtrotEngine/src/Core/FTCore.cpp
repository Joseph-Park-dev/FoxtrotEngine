#include "Core/FTCore.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Core/TemplateFunctions.h"
#include "Core/WindowProcess.h"
#include "Core/Timer.h"
#include "Actors/Actor.h"
#include "Components/SpriteRendererComponent.h"
#include "Managers/KeyInputManager.h"
#include "Managers/SceneManager.h"
#include "Managers/ResourceManager.h"
//#include "Managers/CollisionManager.h"
#include "Managers/EventManager.h"
#include "Managers/UIManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Physics/Physics2D.h"
#include "Physics/ParticleSystem.h"
#include "FileSystem/ChunkLoader.h"

bool FTCore::Initialize()
{
	if (!InitializeWindow())
		return false;
	if (!InitFoxtrotRenderer_D3D11())
		return false;
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

	// 윈도우를 만들때 위에서 계산한 wr 사용
	mWindow = CreateWindow(wc.lpszClassName, mWindowTitle.c_str(), WS_OVERLAPPEDWINDOW,
		100,                // 윈도우 좌측 상단의 x 좌표
		100,                // 윈도우 좌측 상단의 y 좌표
		wr.right - wr.left, // 윈도우 가로 방향 해상도
		wr.bottom - wr.top, // 윈도우 세로 방향 해상도
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
	Camera::GetInstance()->Initialize(mGameRenderer);
	ResourceManager::GetInstance()->Initialize(mGameRenderer);
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
	ParticleSystem::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime);
	Camera::GetInstance()->Update(deltaTime);
}

void FTCore::GenerateOutput()
{
	MSG msg = {};
	InvalidateRect(mWindow, NULL, true);
	if (PeekMessage(&msg, mWindow, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UpdateWindow(mWindow);

	SceneManager::GetInstance()->Render(GetGameRenderer());
	ParticleSystem::GetInstance()->Render(GetGameRenderer());
	GetGameRenderer()->SwapChainPresent(1, 0);
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
	, mWindowTitle(L"Foxtrot Engine (ver.0.01)")
{}

FTCore::~FTCore()
{}

void FTCore::ShutDown()
{
	SceneManager::GetInstance()->DeleteAll();
	ResourceManager::GetInstance()->DeleteAll();
	FoxtrotRenderer::DestroyRenderer(mGameRenderer);
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

//
//void FTCore::SaveEditorData(std::ofstream& ofs)
//{
//	int editorW = 0; int editorH = 0;
//	SDL_GetWindowSize(mEditorWindow, &editorW, &editorH);
//	FileIOHelper::AddBasicString(ofs, mEditorResolutionKey);
//	FileIOHelper::
//		AddVector2(ofs, FTVector2(editorW, editorH));
//}
// // _DEBUG