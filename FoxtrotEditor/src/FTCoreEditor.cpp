#include "FTCoreEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorLayer.h"
#include "EditorSceneManager.h"

#include "Managers/KeyInputManager.h"
#include "Core/FTCore.h"
#include "Core/Timer.h"
#include "Physics/Physics2D.h"
#include "Managers/CollisionManager.h"
#include "Physics/ParticleSystem.h"
#include "Managers/UIManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Core/WindowProcess.h"
#include "Renderer/Camera.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/SceneManager.h"

bool FTCoreEditor::Initialize()
{
	if (!FTCore::Initialize())
	{
		LogString("ERROR : FTCoreEditor::Initialize()->FTCore initialization failed");
		return false;
	}
	if (!InitGUI())
	{
		LogString("Error : FTCoreEditor - Imgui initialization failed");
		return false;
	}
	return true;
}

void FTCoreEditor::ShutDown()
{
	//SaveEditorData();
	EditorLayer::GetInstance()->ShutDown();
	FTCore::ShutDown();
}

void FTCoreEditor::InitSingletonManagers()
{
	FTCore::InitSingletonManagers();
	ResourceManager::GetInstance()->Initialize(GetGameRenderer());
	EditorSceneManager::GetInstance()->Initialize();
	EditorLayer::GetInstance();
}

// Imgui forwawrd declaration
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT FTCoreEditor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_CLOSE:
	{
		if(!EditorLayer::GetInstance()->GetCurrentFileSaved())
			EditorLayer::GetInstance()->SetErrorType(ErrorType::ChunkNotSaved);
		return 0;
	}
	case WM_SIZE:
	{
		// Reset and resize swapchain
		// std::cout << (UINT)LOWORD(lParam) << " " << (UINT)HIWORD(lParam)
		//          << std::endl;
		mIsResizingWindow = true;
		if (GetGameRenderer())
		{
			SetWindowWidth(UINT(LOWORD(lParam)));
			SetWindowHeight(UINT(HIWORD(lParam)));
		}
		break;
	}
	}
	if (mIsResizingWindow && MOUSE_AWAY(MOUSE::MOUSE_LEFT))
	{
		GetGameRenderer()->ResizeWindow(GetWindowWidth(), GetWindowHeight());
		mIsResizingWindow = false;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void FTCoreEditor::ProcessInput()
{
	MSG msg = {};
	if (PeekMessage(&msg, GetWindow(), 0, 0, PM_REMOVE))
	{
		//EditorCamera2D::GetInstance()->ProcessInput(msg);
	}
	KeyInputManager::GetInstance()->DetectKeyInput();
	KeyInputManager::GetInstance()->DetectMouseInput(msg);
	//KeyInputManager::GetInstance()->DetectGamepadInput();
	EditorSceneManager::GetInstance()->ProcessInput(KeyInputManager::GetInstance());
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void FTCoreEditor::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	if (mIsUpdatingGame)
	{
		EditorSceneManager::GetInstance()->Update(deltaTime);
		Physics2D::GetInstance()->Update();
	}
	else
		EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime);
}

void FTCoreEditor::GenerateOutput()
{
	MSG msg = {};
	InvalidateRect(GetWindow(), NULL, true);
	if (PeekMessage(&msg, GetWindow(), 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UpdateWindow(GetWindow());

	if (mIsUpdatingGame)
		EditorSceneManager::GetInstance()->Render(GetGameRenderer());
	else
		EditorSceneManager::GetInstance()->EditorRender(GetGameRenderer());

	EditorLayer::GetInstance()->Render(GetGameRenderer());
	GetGameRenderer()->SwapChainPresent(1, 0);
}

void FTCoreEditor::ProcessEvent()
{
	EditorSceneManager::GetInstance()->ProcessEvent();
}

FTCoreEditor::FTCoreEditor()
	: FTCore()
	, mIsUpdatingGame(false)
	, mIsResizingWindow(false)
	, mEditorDataFileName()
{}

FTCoreEditor::~FTCoreEditor(){}

bool FTCoreEditor::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.DisplaySize = ImVec2(float(GetWindowWidth()), float(GetWindowHeight()));
	ImGui::StyleColorsDark();
	if (!ImGui_ImplWin32_Init(GetWindow())) {
		LogString("Imgui Wind32 Init failed");
		return false;
	}
	if (!ImGui_ImplDX11_Init(
		GetGameRenderer()->GetDevice().Get(),
		GetGameRenderer()->GetContext().Get()
	)
		) {
		LogString("Imgui DX11 Init failed");
		return false;
	}
	return true;
}
