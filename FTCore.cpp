#include "FTCore.h"

#include <windows.h>

#include "Timer.h"
#include "Actor.h"
#include "SpriteComponent.h"
#include "Ship.h"
#include "KeyInputManager.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "CollisionManager.h"
#include "Camera2D.h"
#include "UIManager.h"
#include "Physics2D.h"
#include "ParticleSystem.h"
#include "ChunkLoader.h"
#include "EventManager.h"
#include "FoxtrotRenderer.h"
#include "WindowProcess.h"
#include "TemplateFunctions.h"

FTCore* FTCore::gPInst = nullptr;

bool FTCore::Initialize()
{
	if (!InitializeWindow())
		return false;
	if (!InitFoxtrotRenderer_D3D11())
		return false;
	InitSingletonManagers();
	InitTimer();
	gPInst = this;
	if (!gPInst)
	{
		LogString("Error: FTCore - instance is null");
		return false;
	}
	return true;
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

FTCore* FTCore::GetInstance()
{
	if (gPInst == nullptr)
	{
		gPInst = new FTCore;
	}
	return gPInst;
}

void FTCore::Release()
{
	if (gPInst != nullptr)
	{
		delete gPInst;
		gPInst = nullptr;
	}
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
	Physics2D::GetInstance()->Init();
	Camera2D::GetInstance()->Init();
	SceneManager::GetInstance()->Init();
	ResourceManager::GetInstance();
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

	Physics2D::GetInstance()->Update();
	SceneManager::GetInstance()->Update(deltaTime);
	SceneManager::GetInstance()->Lateupdate(deltaTime);
	CollisionManager::GetInstance()->Update();
	ParticleSystem::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime);
	Camera2D::GetInstance()->Update(deltaTime);
}

void FTCore::GenerateOutput()
{
	mGameRenderer->SetViewport();
	mGameRenderer->RenderClear();
	SceneManager::GetInstance()->Render(GetGameRenderer());
	/* Essential - Don't delete this
	Camera2D::GetInstance()->Render(mEditorRenderer);
	*/
	CollisionManager::GetInstance()->RenderRay(GetGameRenderer());
	ParticleSystem::GetInstance()->Render(GetGameRenderer());
	GetGameRenderer()->SwapChainPresent(1, 0);
}

void FTCore::ProcessEvent()
{
	SceneManager::GetInstance()->ProcessEvent();
}

void FTCore::ShutDown()
{
	FoxtrotRenderer::DestroyRenderer(mGameRenderer);
	DestroyWindow(mWindow);
	PostQuitMessage(0);
}

LRESULT FTCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
	{
		mIsRunning = false;
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

//#ifdef _DEBUG
//void FTCore::SaveEditorData(std::ofstream& ofs)
//{
//	int editorW = 0; int editorH = 0;
//	SDL_GetWindowSize(mEditorWindow, &editorW, &editorH);
//	FileIOHelper::AddBasicString(ofs, mEditorResolutionKey);
//	FileIOHelper::
//		AddVector2(ofs, FTVector2(editorW, editorH));
//}
//#endif // _DEBUG