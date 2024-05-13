#include "FTCoreEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorLayer.h"
#include "EditorCamera2D.h"

#include "KeyInputManager.h"
#include "SceneManager.h"
#include "Timer.h"
#include "Physics2D.h"
#include "CollisionManager.h"
#include "ParticleSystem.h"
#include "UIManager.h"
#include "FoxtrotRenderer.h"
#include "WindowProcess.h"

FTCoreEditor* FTCoreEditor::gPInst = nullptr;

bool FTCoreEditor::Initialize()
{
	if (!FTCore::Initialize())
	{
		LogString("Error : FTCoreEditor - FTCore initialization failed");
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
	EditorCamera2D::GetInstance();
	EditorLayer::GetInstance();
}

// Imgui forwawrd declaration
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT FTCoreEditor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_DESTROY:
	{
		SetIsRunning(false);
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
		EditorCamera2D::GetInstance()->ProcessInput(msg);
	}
	KeyInputManager::GetInstance()->DetectKeyInput();
	KeyInputManager::GetInstance()->DetectMouseInput(msg);
	//KeyInputManager::GetInstance()->DetectGamepadInput();
	SceneManager::GetInstance()->ProcessInput(KeyInputManager::GetInstance());
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void FTCoreEditor::UpdateGame()
{
	Timer::GetInstance()->Update();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

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
}

void FTCoreEditor::GenerateOutput()
{
	GetGameRenderer()->SetViewportEditor();
	//float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//mEditorRenderer->RenderClear(clearColor);
	if (mIsUpdatingGame)
	{
		SceneManager::GetInstance()->Render(GetGameRenderer());
		/* Essential - Don't delete this
		Camera2D::GetInstance()->Render(mEditorRenderer);
		*/
		ParticleSystem::GetInstance()->Render(GetGameRenderer());
	}
	else
	{
		EditorLayer::GetInstance()->DisplayEditorElements(GetGameRenderer());
	}
	EditorCamera2D::GetInstance()->EditorRender(GetGameRenderer());
	EditorLayer::GetInstance()->Render(GetGameRenderer());
	CollisionManager::GetInstance()->RenderRay(GetGameRenderer());
	GetGameRenderer()->RenderToTexture();
	GetGameRenderer()->SwapChainPresent(1, 0);
}

void FTCoreEditor::ProcessEvent()
{
}

FTCoreEditor::FTCoreEditor()
	: FTCore()
	, mIsUpdatingGame(false)
	, mIsResizingWindow(false)
	, mEditorDataFileName()
{}

FTCoreEditor* FTCoreEditor::GetInstance()
{
	if (gPInst == nullptr)
	{
		gPInst = new FTCoreEditor;
	}
	return gPInst;
}

void FTCoreEditor::Release()
{
	if (gPInst != nullptr)
	{
		delete gPInst;
		gPInst = nullptr;
	}
}

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
		GetGameRenderer()->GetDevice(),
		GetGameRenderer()->GetContext()
	)
		) {
		LogString("Imgui DX11 Init failed");
		return false;
	}
	/*ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetWindowViewport());
	ImGui::EndFrame();*/
	return true;
}
