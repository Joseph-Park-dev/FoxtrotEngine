#include "Manager/AnimationManager.h"
#include "Manager/DebugShapes.h"
#include <cstring>
#include <stdexcept>
#include "Plugin/IPlugin.h"
#include "Factory/IGraphicsFactory.h"
#include "Factory/IInputSysFactory.h"
#include "Plugin/D3D11Exports.h"

#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/Camera.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Renderer/FTRectArea.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/SceneManager.h"
#include "Manager/ResourceManager.h"
#include "Plugin/PluginKey.h"

#include "Component/D3D11Component.h"

namespace D3D11
{
	namespace ChunkKey
	{
		namespace Plugin
		{
			constexpr const char* D3D11 = "D3D11";
		}
	} // namespace ChunkKey

class D3D11Plugin :
	public Core::IPlugin,
	public Core::IGraphicsFactory,
	public Core::IInputSysFactory
{
public:
	/// @brief Registers a component for plugin-managed lifecycle processing.
	/// @param comp Component instance associated with the actor or plugin.
	virtual void RegisterComponent(Common::IComponent* comp) override;
    /// @brief Looks up an optional named interface implemented by this plugin.
    /// @param name Name used to identify the requested object or interface.
    /// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
    void* QueryInterface(const char* name) noexcept override {
        if (std::strcmp(name, "GraphicsFactory") == 0) return static_cast<Core::IGraphicsFactory*>(this);
        if (std::strcmp(name, "InputFactory") == 0) return static_cast<Core::IInputSysFactory*>(this);
        return nullptr;
    }

public:
	/// @brief Creates a native application window with the requested client dimensions and render area.
	/// @param title Window title or dialog caption.
	/// @param width Width of the window, texture, or geometry.
	/// @param height Height of the window, texture, or geometry.
	/// @param renderArea Area of the window used for rendering.
	/// @return Borrowed pointer to the subsystem retained by this plugin.
	/// @note The plugin manages the returned object's lifetime.
	virtual Graphics::IWindow* CreateAppWindow(
		const char*	 title,
		unsigned int width,
		unsigned int height,
		D3D11::FTRectArea*	 renderArea) override;

	/// @brief Creates a native application window with the requested client dimensions and render area.
	/// @param title Window title or dialog caption.
	/// @param width Width of the window, texture, or geometry.
	/// @param height Height of the window, texture, or geometry.
	/// @param rndArea Area used for rendering.
	/// @param wndProc Window procedure receiving native messages.
	/// @param wndProcParams Opaque context forwarded to the window procedure.
	/// @return Borrowed pointer to the subsystem retained by this plugin.
	/// @note The plugin manages the returned object's lifetime.
	virtual Graphics::IWindow* CreateAppWindow(
		const char*	 title,
		unsigned int width,
		unsigned int height,
		D3D11::FTRectArea*	 rndArea,
		WNDPROC		 wndProc,
		void*		 wndProcParams) override;
	/// @brief Creates the graphics renderer associated with the application window.
	/// @param window Window used by the operation.
	/// @return Borrowed pointer to the subsystem retained by this plugin.
	/// @note The plugin manages the returned object's lifetime.
	virtual Graphics::IRenderer*	  CreateRenderer(Graphics::IWindow* window) override;
	/// @brief Creates a camera instance for scene rendering.
	/// @return Borrowed pointer to the subsystem retained by this plugin.
	/// @note The plugin manages the returned object's lifetime.
	virtual Graphics::ICamera*	  CreateCamera() override;
	/// @brief Creates the native input device used by the engine.
	/// @return Borrowed pointer to the subsystem retained by this plugin.
	/// @note The plugin manages the returned object's lifetime.
	virtual InputSystem::IInputDevice* CreateInputDevice() override;

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void SaveProperties(std::ofstream& ofs) override;
	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// ///////////////////////////////
	/// /// Initialization Phase //////
	/// ///////////////////////////////
	virtual void Initialize() override;
	/// @brief Updates the up used by subsequent operations.
	virtual void Setup() override;

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	/// ////////////////////
	/// /// Game Loop //////
	/// ////////////////////
	virtual void ProcessInput() override;
	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	virtual void Update(float deltaTime) override;
	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	/// @param deltaTime Elapsed frame time in seconds.
	virtual void LateUpdate(float deltaTime) override;
	/// @brief Submits this object's graphics work for the current frame.
	virtual void Render() override;
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	virtual void ProcessEvent() override;

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// ////////////////////////////
	/// /// Termination Phase //////
	/// ////////////////////////////
	virtual void ShutDown() override;

public:
	/// @brief Initializes the Direct3D plugin's component and graphics service state.
	/// @param name Name used to identify the requested object or interface.
	D3D11Plugin(const char* name);
	/// @brief Releases the resources managed by this instance during destruction.
	~D3D11Plugin() override;

protected:
	/// @brief Restores plugin manager state from the chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	virtual void LoadManagerData(std::ifstream& ifs) override;
	/// @brief Restores plugin resource state from the chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	virtual void LoadResourceData(std::ifstream& ifs) override;

private:
	Common::FTDS::DynamicArray<D3D11::D3D11InputDevice*>* mInputDevices;
	D3D11::D3D11Renderer*								  mRenderer;
	Common::FTDS::DynamicArray<D3D11::D3D11Window*>*	  mWindows;
	D3D11::Camera*										  mCamera;

	Common::FTDS::DynamicArray<D3D11::D3D11Component*>* mRegisteredComps;
};

/// @brief Creates a camera instance for scene rendering.
/// @return Borrowed pointer to the subsystem retained by this plugin.
/// @note The plugin manages the returned object's lifetime.
Graphics::ICamera* D3D11Plugin::CreateCamera()
{
	mCamera = DBG_NEW D3D11::Camera;
	return mCamera;
}

/// @brief Creates the native input device used by the engine.
/// @return Borrowed pointer to the subsystem retained by this plugin.
/// @note The plugin manages the returned object's lifetime.
InputSystem::IInputDevice* D3D11Plugin::CreateInputDevice()
{
	D3D11::D3D11InputDevice* device = DBG_NEW D3D11::D3D11InputDevice;
	mInputDevices->PushBack(device);
	return device;
}

/// @brief Creates the graphics renderer associated with the application window.
/// @param window Window used by the operation.
/// @return Borrowed pointer to the subsystem retained by this plugin.
/// @note The plugin manages the returned object's lifetime.
Graphics::IRenderer* D3D11Plugin::CreateRenderer(Graphics::IWindow* window)
{
	D3D11::D3D11Renderer* renderer = DBG_NEW D3D11::D3D11Renderer(window);
	mRenderer					   = renderer;
	return renderer;
}

/// @brief Registers a component for plugin-managed lifecycle processing.
/// @param comp Component instance associated with the actor or plugin.
void D3D11Plugin::RegisterComponent(Common::IComponent* comp)
{
	if (auto* backend = dynamic_cast<D3D11::D3D11Component*>(comp)) mRegisteredComps->PushBack(backend);
}

/// @brief Creates a native application window with the requested client dimensions and render area.
/// @param title Window title or dialog caption.
/// @param width Width of the window, texture, or geometry.
/// @param height Height of the window, texture, or geometry.
/// @param rndArea Area used for rendering.
/// @return Borrowed pointer to the subsystem retained by this plugin.
/// @note The plugin manages the returned object's lifetime.
Graphics::IWindow* D3D11Plugin::CreateAppWindow(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea)
{
	D3D11::D3D11Window* window = DBG_NEW D3D11::D3D11Window(title, width, height, rndArea);
	mWindows->PushBack(window);
	return window;
}

/// @brief Creates a native application window with the requested client dimensions and render area.
/// @param title Window title or dialog caption.
/// @param width Width of the window, texture, or geometry.
/// @param height Height of the window, texture, or geometry.
/// @param rndArea Area used for rendering.
/// @param wndProc Window procedure receiving native messages.
/// @param wndProcParams Opaque context forwarded to the window procedure.
/// @return Borrowed pointer to the subsystem retained by this plugin.
/// @note The plugin manages the returned object's lifetime.
Graphics::IWindow* D3D11Plugin::CreateAppWindow(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea, WNDPROC wndProc, void* wndProcParams)
{
	D3D11::D3D11Window* window = 
		DBG_NEW D3D11::D3D11Window(title, width, height, rndArea, wndProc, static_cast<D3D11::WNDPROC_Params*>(wndProcParams));
	mWindows->PushBack(window);
	return window;
}

/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
void D3D11Plugin::Initialize()
{
#ifndef FOXTROT_EDITOR
	if (mWindows->GetSize() == 0)
	{
		D3D11::FTRectArea* area = DBG_NEW D3D11::FTRectArea(0.f, 0.f, 1280.f, 720.f);
		CreateAppWindow("Foxtrot", 1280, 720, area);
		if (0 < mWindows->GetSize())
		{
			CreateRenderer(mWindows->At(0));
			if (mWindows->At(0))
				mWindows->At(0)->Initialize(SW_SHOW);
		}
		mCamera = static_cast<D3D11::Camera*>(CreateCamera());
		mCamera->Initialize(mWindows->At(0), 64, 1.8f);
		CreateInputDevice();
	}
#endif

	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		(*iter)->Initialize();
}

/// @brief Updates the up used by subsequent operations.
void D3D11Plugin::Setup()
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		(*iter)->Setup();
}

/// @brief Dispatches input for the current frame to the relevant engine objects.
void D3D11Plugin::ProcessInput()
{
	size_t i = 0;
	for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
	{
		if (!(*iter))
			mWindows->Erase(i);
		++i;
	}

	for (auto input = mInputDevices->Begin(); input != mInputDevices->End(); ++input)
	{
        // Poll once per device/frame, using the focused window's client space.
        auto* target = mWindows->GetSize() ? mWindows->At(0) : nullptr;
        for (auto win = mWindows->Begin(); win != mWindows->End(); ++win)
            if ((*win)->GetHandle() == GetForegroundWindow()) target = *win;
        if (target) (*input)->Update(target);
	}
}

/// @brief Advances frame-dependent state using the current time step.
/// @param deltaTime Elapsed frame time in seconds.
void D3D11Plugin::Update(float deltaTime)
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->GetIsActive())
			continue;
		(*iter)->Update(deltaTime);
	}
}

/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
/// @param deltaTime Elapsed frame time in seconds.
void D3D11Plugin::LateUpdate(float deltaTime)
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->GetIsActive())
			continue;
		(*iter)->LateUpdate(deltaTime);
	}
}

/// @brief Submits this object's graphics work for the current frame.
void D3D11Plugin::Render()
{
#ifdef FOXTROT_EDITOR
    return; // Editor owns presentation of its windows.
#endif
	for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
	{
		// mGameRenderer->RenderClear(mWindow);
		(*iter)->BeginRender(mRenderer);
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->Render(mRenderer, mCamera);
		}
		(*iter)->EndRender(mRenderer);
	}
}

/// @brief Provides an empty lifecycle or extension hook for this implementation.
void D3D11Plugin::ProcessEvent()
{
}

/// @brief Provides an empty lifecycle or extension hook for this implementation.
/// @param ofs Output stream receiving the serialized data.
/// @note Writes to the supplied stream at its current position.
void D3D11Plugin::SaveProperties(std::ofstream& ofs)
{
	// Common::FTDS::String dataPath = D3D11::PluginKey::D3D11;
	// dataPath.Append(Common::FileTypes::PLUGIN_DATA);
	// std::ofstream ofs(dataPath.C_Str());

	// if (ofs.good())
	//{
	//	FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::Plugin::PLUGIN_DATA);

	//	mCamera->SaveProperties(ofs);

	//	FileIOHelper::BeginDataPackSave(ofs, Core::ChunkKey::FTWindow::WINDOW_DATA);
	//	for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
	//	{
	//		FileIOHelper::BeginDataPackSave(ofs, (*iter)->GetTitle());
	//		FileIOHelper::SaveUnsignedInt(ofs, Core::ChunkKey::FTWindow::WIDTH, (*iter)->GetWidth());
	//		FileIOHelper::SaveUnsignedInt(ofs, Core::ChunkKey::FTWindow::HEIGHT, (*iter)->GetHeight());

	//		HMODULE			mod	 = GetModuleHandleA(Common::DLLPath::CORE_EDITOR);
	//		FARPROC			proc = GetProcAddress(mod, D3D11::PluginKey::SAVE_PROPERTIES);
	//		FTRECTAREA_SAVE func = reinterpret_cast<FTRECTAREA_SAVE>(proc);
	//		func(ofs, (*iter)->GetRenderArea());
	//		FileIOHelper::EndDataPackSave(ofs, (*iter)->GetTitle());
	//	}
	//	FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::FTWindow::WINDOW_DATA);

	//	FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::Plugin::PLUGIN_DATA);
	//}
}

/// @brief Restores this object's persistent properties from a .chunk stream.
/// @param ifs Input stream positioned at the expected data; reading advances its position.
/// @note Advances the stream position and updates the destination state.
void D3D11Plugin::LoadProperties(std::ifstream& ifs)
{
	LoadManagerData(ifs);
	LoadResourceData(ifs);
}

/// @brief Provides an empty lifecycle or extension hook for this implementation.
void D3D11Plugin::ShutDown()
{
}

/// @brief Initializes the Direct3D plugin's component and graphics service state.
/// @param name Name used to identify the requested object or interface.
/// @note Initializes the :D3D11Plugin base or delegates to its constructor.
D3D11Plugin::D3D11Plugin(const char* name)
	: mRegisteredComps(DBG_NEW Common::FTDS::DynamicArray<D3D11::D3D11Component*>)
	, mInputDevices(DBG_NEW Common::FTDS::DynamicArray<D3D11::D3D11InputDevice*>)
	, mRenderer(nullptr)
	, mWindows(DBG_NEW Common::FTDS::DynamicArray<D3D11::D3D11Window*>)
	, mCamera(nullptr)
{
	// FTRectArea* area = DBG_NEW FTRectArea(0.f, 0.f, 500.f, 500.f);
	// CreateD3D11Window("Hello!", 500, 500, area);
	// CreateInputDevice();
}

/// @brief Releases the resources managed by this instance during destruction.
D3D11Plugin::~D3D11Plugin()
{
	D3D11::ResourceManager::Destroy();
	D3D11::AnimationManager::Destroy();
	D3D11::DebugShapes::Destroy();
	delete mRegisteredComps;
	for (auto it = mInputDevices->Begin(); it != mInputDevices->End(); ++it) delete *it;
	delete mInputDevices;
	delete mCamera;
	if (mRenderer && mRenderer->GetContext()) mRenderer->GetContext()->ClearState();
	for (auto it = mWindows->Begin(); it != mWindows->End(); ++it) delete *it;
	delete mWindows;
	delete mRenderer;
}

/// @brief Restores plugin manager state from the chunk stream.
/// @param ifs Input stream positioned at the expected data; reading advances its position.
void D3D11Plugin::LoadManagerData(std::ifstream& ifs)
{
	mCamera->LoadProperties(ifs);
}

/// @brief Restores plugin resource state from the chunk stream.
/// @param ifs Input stream positioned at the expected data; reading advances its position.
void D3D11Plugin::LoadResourceData(std::ifstream& ifs)
{
	D3D11::ResourceManager::GetInstance()->LoadDefaultResources(mRenderer);
	D3D11::ResourceManager::GetInstance()->LoadResourcesFromChunk(ifs, mRenderer);
}
} // namespace D3D11

#include "FTDS/Static/FTString.h"
#include "Component/Animator.h"
#include "Component/MeshRenderer.h"
#include "Component/SpineAnimator.h"
#include "Component/SpriteRenderer.h"
#include "Component/TileMapRenderer.h"

extern "C"
{
	/// @brief Allocates the plugin implementation exported by this module.
	/// @param name Name used to identify the requested object or interface.
	/// @return Created plugin instance or resource.
	D3D11_API Core::IPlugin* CreatePlugin(const char* name)
	{
		return DBG_NEW D3D11::D3D11Plugin(name);
	}

	/// @brief Constructs the requested component type for an actor.
	/// @param plugin Plugin supplying component or lifecycle services.
	/// @param actor Actor participating in this operation.
	/// @param name Name used to identify the requested object or interface.
	/// @return Created component instance or resource.
	D3D11_API Common::IComponent* CreateComponent(Common::IPlugin* plugin, Common::IActor* actor, Common::FTDS::String& name)
	{
		// comp->LoadProperties();
		// NEED TO MAKE COMPONENT MANAGER.

		Core::IComponent* comp = nullptr;

		if (name.Equal(D3D11::ChunkKey::Animator::NAME))
			comp = actor->GetData()->AddComponent<D3D11::Animator>(plugin);

		else if (name.Equal(D3D11::ChunkKey::SpriteRenderer::NAME))
			comp = actor->GetData()->AddComponent<D3D11::SpriteRenderer>(plugin);

		else if (name.Equal(D3D11::ChunkKey::SpineAnimator::NAME))
			comp = actor->GetData()->AddComponent<D3D11::SpineAnimator>(plugin);

		else if (name.Equal(D3D11::ChunkKey::TileMapRenderer::NAME))
			comp = actor->GetData()->AddComponent<D3D11::TileMapRenderer>(plugin);

		return comp;
	}
}
