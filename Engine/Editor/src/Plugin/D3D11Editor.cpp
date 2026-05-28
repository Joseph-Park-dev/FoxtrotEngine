// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Plugin/IPlugin.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <Windows.h>
#include <iostream>

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "Manager/DirectoryHelper.h"
#include "EditorCamera.h"
#include "Manager/ResourceManager.h"
#include "EditorUtils.h"

#include "Debugging/DebugFuncs.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/ViewportRenderer.h"
#include "Renderer/FTRectArea.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Plugin/PluginKey.h"
#include "FileSystem/DLLPath.h"
#include "Plugin/GetFunc.h"

#include <../../D3D11/include/Plugin/PluginKey.h>

using namespace Editor;

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFOEX mi;
	mi.cbSize = sizeof(mi);
	if (GetMonitorInfo(hMonitor, &mi))
	{
		if (!(mi.dwFlags & MONITORINFOF_PRIMARY))
		{
			// This is a secondary monitor
			std::wcout << L"Secondary monitor: " << mi.szDevice << std::endl;
			// Save hMonitor somewhere if needed
			*(HMONITOR*)dwData = hMonitor;
			return FALSE; // stop after finding one
		}
	}
	return TRUE; // continue enumeration
}

class D3D11Editor :
	public Core::IPlugin
{
public:
	void Initialize() override;

public:
	bool				GetIsUpdatingGame() const { return mIsUpdatingGame; }
	D3D11::D3D11Window* GetEditorWindow() { return mEditorWin; }

	void SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

public:
	D3D11Editor();
	~D3D11Editor() override;

public:
	static Core::CHUNK_IS_SAVED_FUNC gGetChunkISSavedFunc;

private:
	D3D11::D3D11Window*		 mEditorWin;
	D3D11::D3D11Window*		 mGameWin;
	D3D11::D3D11Renderer*	 mRenderer;
	D3D11::D3D11InputDevice* mInputDevice;
	D3D11::WNDPROC_Params*	 wndprocParams;
	D3D11::ViewportRenderer* mViewport;

	Editor::EditorCamera* mEditorCamera;

	bool				  mIsUpdatingGame;
	bool				  mIsResizingWindow;
	Common::FTDS::String* mEditorDataFileName;

private:
	// These member functions are to be included in Initialize()
	bool InitGUI();

private:
	// GameLoop functions
	void ProcessInput() override;
	void Update(float deltaTime) override;
	void Render() override;

	// static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

void D3D11Editor::Initialize()
{
	gGetChunkISSavedFunc = GetFunc<Core::CHUNK_IS_SAVED_FUNC>(DLLPath::CORE_EDITOR, Core::ProcName::GetChunkIsSaved);

	if (mEditorWin)
	{
		delete mEditorWin;
		mEditorWin = nullptr;
	}

	if (mGameWin)
	{
		delete mGameWin;
		mGameWin = nullptr;
	}

	D3D11::CREATE_WINDOW_PROC	 createWindowFunc	   = GetFunc<D3D11::CREATE_WINDOW_PROC>(DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_D3D11_WINDOW);
	D3D11::CREATE_RENDERER		 createRendererFunc	   = GetFunc<D3D11::CREATE_RENDERER>(DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_RENDERER);
	D3D11::CREATE_INPUTDEVICE	 createInputDeviceFunc = GetFunc<D3D11::CREATE_INPUTDEVICE>(DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_INPUTDEVICE);
	Core::FTRECTAREA_CONSTRUCTOR createRectAreaFunc	   = GetFunc<Core::FTRECTAREA_CONSTRUCTOR>(DLLPath::CORE_EDITOR, Core::PluginKey::CREATE_FTRECTAREA);
	D3D11::CREATE_VP_RENDERER	 createVPRendererFunc  = GetFunc<D3D11::CREATE_VP_RENDERER>(DLLPath::CORE_EDITOR, D3D11::PluginKey::CREATE_VP_RENDERER);

	Core::FTRectArea*		rndArea = createRectAreaFunc(0.f, 0.f, 1280.f, 720.f, 0.f);
	wndprocParams					= DBG_NEW D3D11::WNDPROC_Params{ mEditorWin, mInputDevice, mRenderer, &mIsResizingWindow };
	mEditorWin						= createWindowFunc("Foxtrot Editor", 3840, 2160, rndArea, WinProc, wndprocParams);

	mRenderer = createRendererFunc(mEditorWin);
	mViewport = createVPRendererFunc();
	mViewport->InitializeTexture(mRenderer, ImVec2(1280.f, 720.f));

	rndArea->Set(0.f, 0.f, 1920.f, 1080.f, 0.f);
	mGameWin = createWindowFunc("Game", 1920, 1080, rndArea, WinProc, wndprocParams);

	if (!mGameWin->CreateSwapChain(mRenderer))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize SwapChain");
		return;
	}

	if (!mEditorWin->InitializeWindowRenderer(mRenderer))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize Editor FTWindow Renderer");
		return;
	}

	if (!mGameWin->InitializeWindowRenderer(mRenderer))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow Renderer");
		return;
	}

	// Camera::GetInstance()->Initialize(GetGameWindow(), 64.f, 1.8f);
	mEditorCamera = DBG_NEW Editor::EditorCamera;
	mEditorCamera->Initialize(mEditorWin, 64.f, 1.8f);
	// D3D11::DebugShapes::GetInstance()->GetCameraRect()->Initialize(GetGameRenderer());

	mInputDevice = createInputDeviceFunc();

	if (!InitGUI())
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize ImGui");
		return;
	}
	Editor::EditorLayer::GetInstance()->Initialize(mRenderer);

	// Look for secondary window to display editor window.
	HMONITOR secondaryMon = MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY);
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&secondaryMon);

	MONITORINFO hMonitorInfo;
	hMonitorInfo.cbSize = sizeof(hMonitorInfo);
	GetMonitorInfo(secondaryMon, &hMonitorInfo);

	RECT r = hMonitorInfo.rcMonitor;

	SetWindowPos(
		mEditorWin->GetHandle(),
		NULL,
		r.left,
		r.top,
		mEditorWin->GetWidth(),
		mEditorWin->GetHeight(),
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void D3D11Editor::ProcessInput()
{
	EditorCamera::GetInstance()->ProcessInput(mInputDevice);
}

void D3D11Editor::Render()
{
	// if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
	//{
	//	EditorSceneManager::GetInstance()->Render(renderer);
	//	// EditorSceneManager::GetInstance()->EditorRender(renderer);
	//	DebugShapes::GetInstance()->Render(renderer);
	//	LightManager::GetInstance()->Render(renderer, Camera::GetInstance());
	// }
	// GetGameWindow()->EndRender(renderer);
	// GetGameWindow()->GetSwapChain()->Present(1, 0);

	//// Renders the editor window.
	// mEditorWindow->BeginRender(renderer);
	// EditorLayer::GetInstance()->Render(renderer);
	// mEditorWindow->EndRender(renderer);

	mRenderer->GetViewportRenderer()->BeginRender(mRenderer);
	mRenderer->GetViewportRenderer()->DrawOnTexture(mRenderer);
	mRenderer->GetViewportRenderer()->EndRender(mRenderer);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void D3D11Editor::Update(float deltaTime)
{
	// if (mIsUpdatingGame)
	//{
	//	EditorSceneManager::GetInstance()->Update(deltaTime);
	//	EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
	//	SoundManager::GetInstance()->Update();
	//	Physics2D::GetInstance()->Update();
	//	CollisionManager::GetInstance()->Update();
	//	UIManager::GetInstance()->Update(deltaTime, mEditorWindow->GetInputDevice());
	// }
	// else
	//	EditorSceneManager::GetInstance()->EditorUpdate(deltaTime);
	D3D11::Camera::GetInstance()->Update(deltaTime);
	EditorCamera::GetInstance()->Update(deltaTime);
	EditorLayer::GetInstance()->Update(deltaTime, mEditorWin, mInputDevice, mRenderer);
}

D3D11Editor::D3D11Editor()
	: mEditorWin(nullptr)
	, mGameWin(nullptr)
	, mRenderer(nullptr)
	, mInputDevice(nullptr)
	, wndprocParams(nullptr)
	, mViewport(nullptr)

	, mIsUpdatingGame(false)
	, mIsResizingWindow(false)
	, mEditorDataFileName(DBG_NEW Common::FTDS::String)
{
}

D3D11Editor::~D3D11Editor()
{
	CommandHistory::GetInstance()->ShutDown();
	EditorSceneManager::GetInstance()->GetCurrentScene()->DeleteAll();
	EditorLayer::GetInstance()->ShutDown();

	// FontManager::GetInstance()->Destroy();
	EditorCamera::GetInstance()->Destroy();
	CommandHistory::GetInstance()->Destroy();
	// DebugShapes::GetInstance()->Destroy();
	EditorLayer::GetInstance()->Destroy();
	EditorSceneManager::GetInstance()->Destroy();
	EditorChunkLoader::GetInstance()->Destroy();
	// FTCore::ShutDown();
	// delete mEditorWindow;
}

bool D3D11Editor::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.DisplaySize			   = ImVec2(float(mEditorWin->GetWidth()), float(mEditorWin->GetHeight()));
	io.ConfigDpiScaleFonts	   = true;
	io.ConfigDpiScaleViewports = true;
	// io.FontGlobalScale		   = 1.5f;

	// ImGuiStyle& style = ImGui::GetStyle();
	// style.ScaleAllSizes(2.0f);

	ImGui::StyleColorsDark();
	if (!ImGui_ImplWin32_Init(mEditorWin->GetHandle()))
	{
		printf("Imgui Wind32 Init failed");
		return false;
	}
	if (!ImGui_ImplDX11_Init(
			mRenderer->GetDevice().Get(),
			mRenderer->GetContext().Get()))
	{
		printf("Imgui DX11 Init failed");
		return false;
	}
	return true;
}

LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	D3D11::WNDPROC_Params* params = nullptr;
	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		params	 = static_cast<D3D11::WNDPROC_Params*>(cs->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(params));
	}
	else
	{
		// Retrieve the pointer on every message
		params = reinterpret_cast<D3D11::WNDPROC_Params*>(
			GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	switch (msg)
	{
		case WM_CLOSE:
		{
			if (!D3D11Editor::gGetChunkISSavedFunc())
				EditorLayer::GetInstance()->SetErrorType(ErrorType::ChunkNotSaved);
			else
				Engine::GetInstance()->SetIsRunning(false);
			return 0;
		}
		case WM_SIZE:
		{
			if (params->Window)
			{
				*params->IsResizingWin = true;
				params->Window->SetWidth(UINT(LOWORD(lParam)));
				params->Window->SetHeight(UINT(HIWORD(lParam)));
			}
			break;
		}
	}
	if (params->IsResizingWin && params->InputDevice->MOUSE_AWAY(D3D11::MOUSE::MOUSE_LEFT))
	{
		if (params->Renderer)
		{
			params->Window->ResizeWindow(params->Renderer);
			*params->IsResizingWin = false;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}