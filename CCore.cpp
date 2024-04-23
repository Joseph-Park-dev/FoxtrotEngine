#include "CCore.h"
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

#ifdef _DEBUG
#include "EditorLayer.h"
#include "EditorCamera2D.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#endif // _DEBUG

CCore* CCore::gPInst = nullptr;
// RegisterClassEx()에서 실제로 등록될 콜백 함수
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	// g_appBase를 이용해서 간접적으로 멤버 함수 호출
	return CCore::GetInstance()->MsgProc(hWnd, msg, wParam, lParam);
}

CCore::CCore()
	: mGameTitle(L"Cat Blasters! ver.0.01")
	//, mGameviewWindow(nullptr)
	, mIsRunning(true)
	, mResolution(Vector2(1920.f, 1080.f))
#ifdef _DEBUG
	, mEditorWindow(nullptr)
	, mIsUpdatingGame(false)
	, mEditorWidth(1920)
	, mEditorHeight(1080)
	, mEditorDataFileName("EditorData.json")
	, mEditorResolutionKey("EditorResolution")
#endif
{}

CCore::~CCore()
{}

CCore* CCore::GetInstance()
{
	if (gPInst == nullptr)
	{
		gPInst = new CCore;
	}
	return gPInst;
}

void CCore::Release()
{
	if (gPInst != nullptr)
	{
		delete gPInst;
		gPInst = nullptr;
	}
}

bool CCore::Initialize()
{
	if (!InitMainWindow())
		return false;
	if (!InitFoxtrotRenderer_D3D11())
		return false;
	if (!InitGUI())
		return false;
	InitSingletonManagers();
	InitTimer();
	return true;
}

bool CCore::InitMainWindow()
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
		mGameTitle.c_str(), // lpszClassName, L-string
		NULL			 
	};
	if (!RegisterClassEx(&wc)) 
	{
		LogString("RegisterClassEx() failed.");
		return false;
	}
	RECT wr = { 0, 0, mEditorWidth, mEditorHeight };

	// 필요한 윈도우 크기(해상도) 계산
	// wr의 값이 바뀜
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	// 윈도우를 만들때 위에서 계산한 wr 사용
	mEditorWindow = CreateWindow(wc.lpszClassName, mGameTitle.c_str(), WS_OVERLAPPEDWINDOW,
		100,                // 윈도우 좌측 상단의 x 좌표
		100,                // 윈도우 좌측 상단의 y 좌표
		wr.right - wr.left, // 윈도우 가로 방향 해상도
		wr.bottom - wr.top, // 윈도우 세로 방향 해상도
		NULL, NULL, wc.hInstance, NULL);
	if (!mEditorWindow) 
	{
		LogString("CreateWindow() failed.");
		return false;
	}

	ShowWindow(mEditorWindow, SW_SHOWDEFAULT);
	UpdateWindow(mEditorWindow);

	return true;
}

bool CCore::InitFoxtrotRenderer_D3D11()
{
	mEditorRenderer = FoxtrotRenderer::CreateRenderer(mEditorWindow, mEditorWidth, mEditorHeight);
	if (!mEditorRenderer)
		return false;
	return true;
}

bool CCore::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.DisplaySize = ImVec2(float(mEditorWidth), float(mEditorHeight));
	ImGui::StyleColorsDark();

	if (!ImGui_ImplWin32_Init(mEditorWindow)) {
		LogString("Imgui Wind32 Init failed");
		return false;
	}
	if (!ImGui_ImplDX11_Init(
		mEditorRenderer->GetDevice(), 
		mEditorRenderer->GetContext()
	)
	) {
		LogString("Imgui DX11 Init failed");
		return false;
	}
	return true;
}

void CCore::InitSingletonManagers()
{
	Physics2D::GetInstance()->Init();
	Camera2D::GetInstance()->Init();
	SceneManager::GetInstance()->Init();
	ResourceManager::GetInstance();
	UIManager::GetInstance();
	EventManager::GetInstance();
	KeyInputManager::GetInstance();

#ifdef _DEBUG
	EditorCamera2D::GetInstance();
	EditorLayer::GetInstance();
#endif // _DEBUG
}

void CCore::InitTimer()
{
	Timer::GetInstance();
}

void CCore::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();

		ProcessEvent();
	}
}

void CCore::ProcessInput()
{
	MSG msg = {};
	while (PeekMessage(&msg, mEditorWindow, 0, 0, PM_REMOVE))
	{
	#ifdef _DEBUG
		//EditorCamera2D::GetInstance()->ProcessInput(&msg);
	#endif // _DEBUG
	}
	KeyInputManager::GetInstance()->DetectKeyInput();
	/*KeyInputManager::GetInstance()->DetectMouseInput();
	KeyInputManager::GetInstance()->DetectGamepadInput();*/
	SceneManager::GetInstance()->ProcessInput(KeyInputManager::GetInstance());
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}
	/*SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_WINDOWEVENT
			&& event.window.event == SDL_WINDOWEVENT_CLOSE)
		{
			mIsRunning = false;
			break;
		}
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
			case SDL_CONTROLLERDEVICEADDED:
				if (!KeyInputManager::GetInstance()->GetGamepad())
					KeyInputManager::GetInstance()->SetGamepad(SDL_GameControllerOpen(event.cdevice.which));
				SDL_Log("Gamepad added!");
				break;
			case SDL_CONTROLLERDEVICEREMOVED:
				SDL_GameController* controller = KeyInputManager::GetInstance()->GetGamepad();
				if (controller &&
					event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)))
				{
					SDL_GameControllerClose(controller);
					KeyInputManager::GetInstance()->
						SetGamepad(KeyInputManager::GetInstance()->FindGamepad());
					SDL_Log("Gamepad removed!");
				}
				break;
		}*/

void CCore::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

#ifdef _DEBUG
	if (mIsUpdatingGame)
	{
		Physics2D::GetInstance()->Update();
		SceneManager::GetInstance()->Update(deltaTime);
		SceneManager::GetInstance()->Lateupdate(deltaTime);
		CollisionManager::GetInstance()->Update();
		ParticleSystem::GetInstance()->Update(deltaTime);
		UIManager::GetInstance()->Update(deltaTime);
	}
	else
	{
		SceneManager::GetInstance()->EditorUpdate(deltaTime);
		SceneManager::GetInstance()->EditorLateUpdate(deltaTime);
		UIManager::GetInstance()->EditorUpdate(deltaTime);
	}
	Camera2D::GetInstance()->Update(deltaTime);
	EditorCamera2D::GetInstance()->Update(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime);
#else
	SceneManager::GetInstance()->Update(deltaTime);
	SceneManager::GetInstance()->Lateupdate(deltaTime);
	CollisionManager::GetInstance()->Update();
	ParticleSystem::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime);
	Camera2D::GetInstance()->Update(deltaTime);
#endif
}

void CCore::GenerateOutput()
{
#ifdef _DEBUG
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	mEditorRenderer->RenderClear(clearColor);

	if (mIsUpdatingGame)
	{
		SceneManager::GetInstance()->Render(mEditorRenderer);
		/* Essential - Don't delete this
		Camera2D::GetInstance()->Render(mEditorRenderer);
		*/
		ParticleSystem::GetInstance()->Render(mEditorRenderer);
	}
	else
	{
		EditorLayer::GetInstance()->DisplayEditorElements(mEditorRenderer);
	}
	EditorCamera2D::GetInstance()->EditorRender(mEditorRenderer);
	EditorLayer::GetInstance()->Render(mEditorRenderer);
	CollisionManager::GetInstance()->RenderRay(mEditorRenderer);

	mEditorRenderer->BatchRenderTextures();
	mEditorRenderer->SwapChainPresent(1, 0);

#else
	SDL_SetRenderDrawColor(
		mGameRenderer,
		0, 0, 255, 255
	);
	SDL_RenderClear(mGameRenderer);
	SceneManager::GetInstance()->Render(mGameRenderer);
	//Camera2D::GetInstance()->Render(mGameRenderer);
	CollisionManager::GetInstance()->RenderRay(mGameRenderer);
	ParticleSystem::GetInstance()->Render(mGameRenderer);
	SDL_RenderPresent(mGameRenderer);
#endif
}

void CCore::ProcessEvent()
{
	SceneManager::GetInstance()->ProcessEvent();
}

void CCore::ShutDown()
{
#ifdef _DEBUG
	//SaveEditorData();
	EditorLayer::GetInstance()->ShutDown();
	FoxtrotRenderer::DestroyRenderer(mEditorRenderer);
#endif // _DEBUG
	DestroyWindow(mEditorWindow);
	PostQuitMessage(0);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CCore::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
	switch (msg) {
		case WM_DESTROY:
		{
			mIsRunning = false;
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//#ifdef _DEBUG
//void CCore::SaveEditorData(std::ofstream& ofs)
//{
//	int editorW = 0; int editorH = 0;
//	SDL_GetWindowSize(mEditorWindow, &editorW, &editorH);
//	FileIOHelper::AddBasicString(ofs, mEditorResolutionKey);
//	FileIOHelper::
//		AddVector2(ofs, Vector2(editorW, editorH));
//}
//#endif // _DEBUG