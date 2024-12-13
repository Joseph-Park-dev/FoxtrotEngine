#include "FTCoreEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "DebugGeometries.h"
#include "DirectoryHelper.h"

#include "Managers/KeyInputManager.h"
#include "Core/FTCore.h"
#include "Core/Timer.h"
#include "Physics/Physics2D.h"
#include "Physics/ParticleSystem.h"
#include "Managers/UIManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Core/WindowProcess.h"
#include "Renderer/Camera.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/SceneManager.h"
#include "Managers/CollisionManager.h"
#include "EditorChunkLoader.h"

// FTCoreEditor related singleton initializations -> used in Foxtrot Editor Runtime
CommandHistory*		CommandHistory::mInstance = nullptr;
DebugGeometries*	DebugGeometries::mInstance = nullptr;
DirectoryHelper*	DirectoryHelper::mInstance = nullptr;
EditorLayer*		EditorLayer::mInstance = nullptr;
EditorSceneManager* EditorSceneManager::mInstance = nullptr;
EditorChunkLoader*	EditorChunkLoader::mInstance = nullptr;
FTCoreEditor*		FTCoreEditor::mInstance = nullptr;

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
	CommandHistory::GetInstance()->ShutDown();
	EditorSceneManager::GetInstance()->DeleteAll();
	ResourceManager::GetInstance()->DeleteAll();
	EditorLayer::GetInstance()->ShutDown();

	CommandHistory::GetInstance()->Destroy();
	DebugGeometries::GetInstance()->Destroy();
	DirectoryHelper::GetInstance()->Destroy();
	EditorLayer::GetInstance()->Destroy();
	EditorSceneManager::GetInstance()->Destroy();
	EditorChunkLoader::GetInstance()->Destroy();
	FTCore::ShutDown();
}

void FTCoreEditor::InitSingletonManagers()
{
	Physics2D::GetInstance()->Initialize();
	Camera::GetInstance()->Initialize(GetGameRenderer(), 64.f, 1.8f);
	ResourceManager::GetInstance()->Initialize(GetGameRenderer());
	UIManager::GetInstance();
	EventManager::GetInstance();
	KeyInputManager::GetInstance();
	CollisionManager::GetInstance()->Initialize();

	EditorSceneManager::GetInstance()->Initialize();
	EditorLayer::GetInstance();
	DebugGeometries::GetInstance()->Initialize(GetGameRenderer());
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
		if (!CHUNK_IS_SAVED)
			EditorLayer::GetInstance()->SetErrorType(ErrorType::ChunkNotSaved);
		else
			this->SetIsRunning(false);
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
		EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
		Physics2D::GetInstance()->Update();
	}
	else
		EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime);
	Camera::GetInstance()->Update(deltaTime);
	UIManager::GetInstance()->Update(deltaTime);
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

	GetGameRenderer()->RenderToTexture();
	EditorLayer::GetInstance()->Render(GetGameRenderer());
	GetGameRenderer()->SwapChainPresent(1, 0);
}

void FTCoreEditor::ProcessEvent()
{
	EditorSceneManager::GetInstance()->ProcessEvent();
	EventManager::GetInstance()->ProcessEvent();
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
