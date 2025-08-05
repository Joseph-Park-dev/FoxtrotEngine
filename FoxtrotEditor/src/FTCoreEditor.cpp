// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTCoreEditor.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "DirectoryHelper.h"
#include "EditorCamera.h"
#include "ViewportRenderer.h"
#include "EditorChunkLoader.h"
#include "EditorResourceManager.h"
#include "EditorUtils.h"

#include "Managers/DebugShapes.h"
#include "InputSystem/FTInputDevice.h"
#include "Core/FTCore.h"
#include "Core/Timer.h"
#include "Physics/Physics2D.h"
#include "Physics/ParticleSystem.h"
#include "Managers/UIManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Core/WindowProcess.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FTRectArea.h"
#include "WindowSystem/FTWindow.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/SceneManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/SoundManager.h"
#include "ResourceSystem/FTRectangle.h"

// FTCoreEditor related singleton initializations -> used in Foxtrot Editor Runtime
CommandHistory*		   CommandHistory::mInstance		= nullptr;
DirectoryHelper*	   DirectoryHelper::mInstance		= nullptr;
EditorLayer*		   EditorLayer::mInstance			= nullptr;
EditorSceneManager*	   EditorSceneManager::mInstance	= nullptr;
EditorChunkLoader*	   EditorChunkLoader::mInstance		= nullptr;
FTCoreEditor*		   FTCoreEditor::mInstance			= nullptr;
EditorCamera*		   EditorCamera::mInstance			= nullptr;
EditorResourceManager* EditorResourceManager::mInstance = nullptr;

bool FTCoreEditor::Initialize()
{
	if (!FTCore::Initialize())
	{
		Debug::LogError(__LINE__, __FILE__, "FTCoreEditor::Initialize()->FTCore initialization failed");
		return false;
	}

	if (mEditorWindow)
	{
		delete mEditorWindow;
		mEditorWindow = nullptr;
	}
	mEditorWindow = DBG_NEW FTWindow("Foxtrot Editor", 3840, 2160);

	if (!mEditorWindow->InitializeWindow(WndProc_FTEditor))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow");
		return false;
	}

	GetGameRenderer()->InitializeViewport(mEditorWindow, 1280, 720);

	if (!mEditorWindow->CreateSwapChain(GetGameRenderer()))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize SwapChain");
		return false;
	}

	if (!mEditorWindow->InitializeWindowRenderer(GetGameRenderer()))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow Renderer");
		return false;
	}
	EditorCamera::GetInstance()->Initialize(mEditorWindow, 64.f, 1.8f);
	EditorCamera::GetInstance()->GetDebugRect()->Initialize(GetGameRenderer());
	DebugShapes::GetInstance()->SetCameraRect(EditorCamera::GetInstance()->GetDebugRect());

	if (!InitGUI())
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize ImGui");
		return false;
	}
	return true;
}

void FTCoreEditor::ShutDown()
{
	CommandHistory::GetInstance()->ShutDown();
	EditorSceneManager::GetInstance()->DeleteAll();
	EditorLayer::GetInstance()->ShutDown();

	EditorCamera::GetInstance()->Destroy();
	CommandHistory::GetInstance()->Destroy();
	DebugShapes::GetInstance()->Destroy();
	DirectoryHelper::GetInstance()->Destroy();
	EditorLayer::GetInstance()->Destroy();
	EditorSceneManager::GetInstance()->Destroy();
	EditorChunkLoader::GetInstance()->Destroy();
	EditorResourceManager::GetInstance()->Destroy();
	FTCore::ShutDown();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT FTCoreEditor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
		case WM_CLOSE:
		{
			if (!CHUNK_IS_SAVED)
				EditorLayer::GetInstance()->SetErrorType(ErrorType::ChunkNotSaved);
			else
				FTCoreEditor::GetInstance()->SetIsRunning(false);
			return 0;
		}
		case WM_SIZE:
		{
			if (mEditorWindow)
			{
				mIsResizingWindow = true;
				mEditorWindow->SetWidth(UINT(LOWORD(lParam)));
				mEditorWindow->SetHeight(UINT(HIWORD(lParam)));
			}
			break;
		}
	}
	if (mIsResizingWindow && mEditorWindow->MOUSE_AWAY(MOUSE::MOUSE_LEFT))
	{
		mEditorWindow->ResizeWindow(GetGameRenderer());
		mIsResizingWindow = false;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void FTCoreEditor::InitSingletonManagers()
{
	Physics2D::GetInstance()->Initialize();
	Camera::GetInstance()->Initialize(GetGameWindow(), 64.f, 1.8f);
	EditorResourceManager::GetInstance()->Initialize(GetGameRenderer());
	SoundManager::GetInstance()->Initialize();
	UIManager::GetInstance();
	EventManager::GetInstance();
	CollisionManager::GetInstance()->Initialize();
	LightManager::GetInstance()->Initialize(GetGameRenderer());
	DebugShapes::GetInstance()->Initialize(GetGameRenderer());
	AnimationManager::GetInstance()->Initialize(GetGameRenderer());

	EditorSceneManager::GetInstance()->Initialize();
	EditorLayer::GetInstance();
}

void FTCoreEditor::LoadGameData()
{
	LogString("Initializing FTEditor. GameData will not be loaded.");
}

void FTCoreEditor::ProcessInput()
{
	FTCore::ProcessInput();
	mEditorWindow->ProcessInput();

	if (mIsUpdatingGame)
		EditorSceneManager::GetInstance()->ProcessInput(GetGameWindow()->GetInputDevice());

	EditorCamera::GetInstance()->ProcessInput(mEditorWindow->GetInputDevice());
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
		CollisionManager::GetInstance()->Update();
		UIManager::GetInstance()->Update(deltaTime, mEditorWindow->GetInputDevice());
		SoundManager::GetInstance()->LateUpdate();
	}
	else
		EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
	Camera::GetInstance()->Update(deltaTime);
	EditorCamera::GetInstance()->Update(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime);
}

void FTCoreEditor::GenerateOutput()
{
	FoxtrotRenderer* renderer = GetGameRenderer();

	// Renders the gameview window.
	GetGameWindow()->BeginRender(renderer);
	FTVector2 size = GetGameWindow()->GetRenderArea()->GetSize();
	renderer->SetViewport(0, 0, size.x, size.y);

	if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
	{
		EditorSceneManager::GetInstance()->Render(renderer);
		// EditorSceneManager::GetInstance()->EditorRender(renderer);
		DebugShapes::GetInstance()->Render(renderer);
		LightManager::GetInstance()->Render(renderer, Camera::GetInstance());
	}

	GetGameWindow()->EndRender(renderer);

	GetGameRenderer()->GetViewportRenderer()->BeginRender(renderer);
	GetGameRenderer()->GetViewportRenderer()->DrawOnTexture(renderer);
	GetGameRenderer()->GetViewportRenderer()->EndRender(renderer);

	// Renders the editor window.
	mEditorWindow->BeginRender(renderer);
	EditorLayer::GetInstance()->Render(renderer);
	mEditorWindow->EndRender(renderer);

	GetGameWindow()->GetSwapChain()->Present(1, 0);
	mEditorWindow->GetSwapChain()->Present(1, 0);
}

void FTCoreEditor::ProcessEvent()
{
	EventManager::GetInstance()->ProcessEvent();
}

FTCoreEditor::FTCoreEditor()
	: FTCore()
	, mEditorWindow(nullptr)
	, mIsUpdatingGame(false)
	, mIsResizingWindow(false)
	, mEditorDataFileName()
{
}

FTCoreEditor::~FTCoreEditor()
{
	delete mEditorWindow;
}

bool FTCoreEditor::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize			   = ImVec2(float(mEditorWindow->GetWidth()), float(mEditorWindow->GetHeight()));
	io.ConfigDpiScaleFonts	   = true;
	io.ConfigDpiScaleViewports = true;
	io.FontGlobalScale		   = 1.5f;

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(2.0f);

	ImGui::StyleColorsDark();
	if (!ImGui_ImplWin32_Init(mEditorWindow->GetHandle()))
	{
		LogString("Imgui Wind32 Init failed");
		return false;
	}
	if (!ImGui_ImplDX11_Init(
			GetGameRenderer()->GetDevice().Get(),
			GetGameRenderer()->GetContext().Get()))
	{
		LogString("Imgui DX11 Init failed");
		return false;
	}
	return true;
}
