#include "Manager/EditorShapes.h"
#include <stdexcept>
#include "Renderer/Camera.h"
#include "Foxtrot/Runtime/PlatformApi.h"
#include "EditorScene.h"
#include <stdexcept>
#include "ChunkLoader.h"
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
#include "Manager/PluginManager.h"
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

#include "Factory/IGraphicsFactory.h"
#include "Factory/IInputSysFactory.h"

#include "Core/FTCore.h"
#include <../../D3D11/include/Plugin/PluginKey.h>

/// @brief Forwards native window messages to the ImGui Win32 backend.
/// @param hWnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wParam Message-specific Windows parameter.
/// @param lParam Message-specific Windows parameter.
/// @return Forwards native window messages to the ImGui Win32 backend.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
using namespace Editor;

/// @brief Dispatches native window messages for input and window lifecycle handling.
/// @param hwnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wParam Message-specific Windows parameter.
/// @param lParam Message-specific Windows parameter.
/// @return Dispatches native window messages for input and window lifecycle handling.
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
/// @brief Dispatches native window messages to the game window's input and lifecycle handlers.
/// @param hwnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wParam Message-specific Windows parameter.
/// @param lParam Message-specific Windows parameter.
/// @return Dispatches native window messages to the game window's input and lifecycle handlers.
LRESULT CALLBACK GameWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// @brief Processes a monitor encountered during display enumeration.
/// @param hMonitor Monitor handle supplied by display enumeration.
/// @param hdcMonitor Monitor device context supplied by enumeration.
/// @param lprcMonitor Monitor bounds supplied by enumeration.
/// @param dwData Caller context supplied to monitor enumeration.
/// @return Processes a monitor encountered during display enumeration.
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

namespace Editor
{
class D3D11Editor :
	public Core::IPlugin
{
public:
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
	void Initialize() override;

public:
	/// @brief Returns the is updating game used by this d3 d11 editor.
	/// @return Current value of the is updating game flag.
	bool				GetIsUpdatingGame() const { return mIsUpdatingGame; }
	/// @brief Returns the editor window used by this d3 d11 editor.
	/// @return Borrowed access to the editor window.
	D3D11::D3D11Window* GetEditorWindow() { return mEditorWin; }

	/// @brief Updates the is updating game used by subsequent operations.
	/// @param isUpdating Replacement is updating game.
	void SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

public:
	/// @brief Initializes the editor plugin's windows, cameras, and services.
	D3D11Editor();
	/// @brief Releases the resources managed by this instance during destruction.
	~D3D11Editor() override;

public:
	inline static Core::CHUNK_IS_SAVED_FUNC gGetChunkISSavedFunc = nullptr;

private:
	D3D11::D3D11Window*		 mEditorWin;
	D3D11::D3D11Window*		 mGameWin;
	D3D11::D3D11Renderer*	 mRenderer;
	D3D11::D3D11InputDevice* mInputDevice;
	D3D11::WNDPROC_Params*	 wndprocParams;
	D3D11::ViewportRenderer* mViewport;

	D3D11::Camera*		  mGameCamera;
	Editor::EditorCamera* mEditorCamera;

	bool				  mIsUpdatingGame;
	bool				  mIsResizingWindow;
	Common::FTDS::String* mEditorDataFileName;

private:
	// These member functions are to be included in Initialize()
	/// @brief Initializes the ImGui state and platform/rendering backend bindings.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool InitGUI();
    bool mGuiReady = false;

private:
	// GameLoop functions
	/// @brief Registers a component for plugin-managed lifecycle processing.
	/// @param comp Component instance associated with the actor or plugin.
	void RegisterComponent(Common::IComponent* comp) override { (void)comp; }
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (std::ofstream&): reserved by this interface or unused by this implementation.
	/// @note Writes to the supplied stream at its current position.
	void SaveProperties(std::ofstream&) override {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (std::ifstream&): reserved by this interface or unused by this implementation.
	/// @note Advances the stream position and updates the destination state.
	void LoadProperties(std::ifstream&) override {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	void Setup() override {}
	/// @brief Dispatches input for the current frame to the relevant engine objects.
	void ProcessInput() override;
	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	void Update(float deltaTime) override;
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (float): reserved by this interface or unused by this implementation.
	void LateUpdate(float) override {}
	/// @brief Submits this object's graphics work for the current frame.
	void Render() override;
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	void ProcessEvent() override {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	void ShutDown() override {}

protected:
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (std::ifstream&): reserved by this interface or unused by this implementation.
	void LoadManagerData(std::ifstream&) override {}
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @note Unnamed parameter (std::ifstream&): reserved by this interface or unused by this implementation.
	void LoadResourceData(std::ifstream&) override {}

	// static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
void D3D11Editor::Initialize()
{
	gGetChunkISSavedFunc = Core::GetFunc<Core::CHUNK_IS_SAVED_FUNC>(Common::DLLPath::CORE_EDITOR, Core::ProcName::GetChunkIsSaved);

	IPlugin* d3d11Plugin = nullptr;
	if (auto* plugins = Core::GetAvailablePlugins())
		for (auto it = plugins->Begin(); it != plugins->End(); ++it)
			if (*it && (*it)->Key().Equal("D3D11"))
			{
				d3d11Plugin = (*it)->Value();
				break;
			}
	if (!d3d11Plugin)
		throw std::runtime_error("Editor requires the attached D3D11 plugin");

	Core::IGraphicsFactory* graphicsFac = static_cast<Core::IGraphicsFactory*>(d3d11Plugin->QueryInterface("GraphicsFactory"));
	Core::IInputSysFactory* inputFac = static_cast<Core::IInputSysFactory*>(d3d11Plugin->QueryInterface("InputFactory"));
	if (!graphicsFac || !inputFac)
		throw std::runtime_error("D3D11 plugin is missing editor graphics or input services");

	D3D11::CREATE_VP_RENDERER createVPRendererFunc = Core::GetFunc<D3D11::CREATE_VP_RENDERER>(Common::DLLPath::D3D11, Core::ProcNames::D3D11::CREATE_VP_RENDERER);
	// D3D11::CREATE_WINDOW_PROC	 createWindowFunc	   = Core::GetFunc<D3D11::CREATE_WINDOW_PROC>(Common::DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_D3D11_WINDOW);
	// D3D11::CREATE_RENDERER		 createRendererFunc	   = Core::GetFunc<D3D11::CREATE_RENDERER>(Common::DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_RENDERER);
	// D3D11::CREATE_INPUTDEVICE	 createInputDeviceFunc = Core::GetFunc<D3D11::CREATE_INPUTDEVICE>(Common::DLLPath::D3D11_EDITOR, D3D11::PluginKey::CREATE_INPUTDEVICE);

	D3D11::FTRectArea* rndArea = DBG_NEW D3D11::FTRectArea(0.f, 0.f, 1280.f, 720.f, 0.f);

	wndprocParams = DBG_NEW D3D11::WNDPROC_Params{ mEditorWin, mInputDevice, mRenderer, &mIsResizingWindow };
	mEditorWin	  = static_cast<D3D11::D3D11Window*>(graphicsFac->CreateAppWindow("Foxtrot Editor", 3840, 2160, rndArea, WinProc, wndprocParams));
	mRenderer	  = reinterpret_cast<D3D11::D3D11Renderer*>(graphicsFac->CreateRenderer(mEditorWin));

	mViewport = createVPRendererFunc();
	mViewport->InitializeTexture(mRenderer, 1280.f, 720.f);

	rndArea = DBG_NEW D3D11::FTRectArea(0.f, 0.f, 1920.f, 1080.f, 0.f);
	mGameWin = static_cast<D3D11::D3D11Window*>(graphicsFac->CreateAppWindow("Game", 1920, 1080, rndArea, GameWinProc, nullptr));

	if (!mGameWin->CreateSwapChain(mRenderer))
	{
		Common::Debug::LogError(__LINE__, __FILE__, "Failed to Initialize SwapChain");
		return;
	}

	if (!mEditorWin->InitializeWindowRenderer(mRenderer))
	{
		Common::Debug::LogError(__LINE__, __FILE__, "Failed to Initialize Editor FTWindow Renderer");
		return;
	}

	if (!mGameWin->InitializeWindowRenderer(mRenderer))
	{
		Common::Debug::LogError(__LINE__, __FILE__, "Failed to Initialize FTWindow Renderer");
		return;
	}

	mGameCamera = static_cast<D3D11::Camera*>(graphicsFac->CreateCamera());
    mGameCamera->Initialize(mGameWin, 64, 1.8f);
    EditorSceneManager::GetInstance()->Initialize(new EditorScene);
	// Camera::GetInstance()->Initialize(GetGameWindow(), 64.f, 1.8f);
	mEditorCamera = DBG_NEW Editor::EditorCamera;
	mEditorCamera->Initialize(mEditorWin, 64, 1.8f);
	// D3D11::DebugShapes::GetInstance()->GetCameraRect()->Initialize(GetGameRenderer());

	mInputDevice					 = reinterpret_cast<D3D11::D3D11InputDevice*>(inputFac->CreateInputDevice());

	if (!InitGUI())
	{
		throw std::runtime_error("Failed to initialize ImGui");
	}
	mGuiReady = true;
    D3D11::SetEditorGuiContext(ImGui::GetCurrentContext());
    wndprocParams->Window = mEditorWin;
    wndprocParams->Renderer = mRenderer;
    wndprocParams->InputDevice = mInputDevice;
    Editor::EditorLayer::GetInstance()->SetViewportRenderer(mViewport);
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

/// @brief Dispatches input for the current frame to the relevant engine objects.
void D3D11Editor::ProcessInput()
{
	mEditorCamera->ProcessInput(mInputDevice);
}

/// @brief Submits this object's graphics work for the current frame.
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

	mViewport->BeginRender(mRenderer);
	mViewport->DrawOnTexture(mRenderer);
	mViewport->EndRender(mRenderer);
    mEditorWin->BeginRender(mRenderer);
    EditorLayer::GetInstance()->Render();
    mEditorWin->EndRender(mRenderer);
}



/// @brief Advances frame-dependent state using the current time step.
/// @param deltaTime Elapsed frame time in seconds.
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
	// mGameCamera->Update(deltaTime);
	Core::ICamera* gameCam = reinterpret_cast<Core::ICamera*>(mGameCamera);
	mEditorCamera->Update(gameCam);
	EditorLayer::GetInstance()->Update(deltaTime, mEditorWin, mInputDevice, mRenderer, gameCam, mEditorCamera);
}

/// @brief Initializes the editor plugin's windows, cameras, and services.
/// @note Initializes the :D3D11Editor base or delegates to its constructor.
D3D11Editor::D3D11Editor()
	: mEditorWin(nullptr)
	, mGameWin(nullptr)
	, mRenderer(nullptr)
	, mInputDevice(nullptr)
	, wndprocParams(nullptr)
	, mViewport(nullptr)
	, mGameCamera(nullptr)
	, mEditorCamera(nullptr)

	, mIsUpdatingGame(false)
	, mIsResizingWindow(false)
	, mEditorDataFileName(DBG_NEW Common::FTDS::String)
{
}

/// @brief Releases the resources managed by this instance during destruction.
D3D11Editor::~D3D11Editor()
{
    if (mEditorWin) FtDetachWindowCallback(mEditorWin->GetHandle());
    if (mGameWin) FtDetachWindowCallback(mGameWin->GetHandle());
    CommandHistory::GetInstance()->ShutDown();
    CommandHistory::Destroy();
    EditorSceneManager::Destroy();
    ChunkLoader::Destroy();
    if (mGuiReady) { D3D11::SetEditorGuiContext(nullptr); EditorLayer::GetInstance()->ShutDown(); }
    EditorLayer::Destroy();
    EditorShapes::Destroy();
    delete mEditorCamera;
    D3D11::DestroyViewportRenderer(mViewport);
    delete wndprocParams;
    delete mEditorDataFileName;
    // Windows, device, game camera and input are borrowed from D3D11's factory.
}

/// @brief Initializes the ImGui state and platform/rendering backend bindings.
/// @return True if the operation succeeds or the tested condition holds; otherwise false.
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
		ImGui::DestroyContext();
		return false;
	}
	if (!ImGui_ImplDX11_Init(
			mRenderer->GetDevice().Get(),
			mRenderer->GetContext().Get()))
	{
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		return false;
	}
	return true;
}
} // namespace Editor

/// @brief Dispatches native window messages for input and window lifecycle handling.
/// @param hwnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wParam Message-specific Windows parameter.
/// @param lParam Message-specific Windows parameter.
/// @return Dispatches native window messages for input and window lifecycle handling.
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
				Core::FTCore::GetInstance()->SetIsRunning(false);
			return 0;
		}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

#include "Plugin/EditorExports.h"

extern "C"
{
	/// @brief Allocates the plugin implementation exported by this module.
	/// @param name Name used to identify the requested object or interface.
	/// @return Created plugin instance or resource.
	EDITOR_API Common::IPlugin* CreatePlugin(const char* name)
	{
		(void)name;
		return DBG_NEW Editor::D3D11Editor();
	}
}
// Preserve the unsaved-scene close check without feeding a second HWND's mouse
// coordinates into the editor window's ImGui backend.
/// @brief Dispatches native window messages to the game window's input and lifecycle handlers.
/// @param hwnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wParam Message-specific Windows parameter.
/// @param lParam Message-specific Windows parameter.
/// @return Dispatches native window messages to the game window's input and lifecycle handlers.
LRESULT CALLBACK GameWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_CLOSE) return WinProc(hwnd, msg, wParam, lParam);
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
