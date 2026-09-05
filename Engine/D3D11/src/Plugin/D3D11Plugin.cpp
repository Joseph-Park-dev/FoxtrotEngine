#pragma once
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
	virtual void RegisterComponent(Common::IComponent* comp) override;

public:
	virtual Graphics::IWindow* CreateAppWindow(
		const char*	 title,
		unsigned int width,
		unsigned int height,
		D3D11::FTRectArea*	 renderArea) override;

	virtual Graphics::IWindow* CreateAppWindow(
		const char*	 title,
		unsigned int width,
		unsigned int height,
		D3D11::FTRectArea*	 rndArea,
		WNDPROC		 wndProc,
		void*		 wndProcParams) override;
	virtual Graphics::IRenderer*	  CreateRenderer(Graphics::IWindow* window) override;
	virtual Graphics::ICamera*	  CreateCamera() override;
	virtual InputSystem::IInputDevice* CreateInputDevice() override;

	void SaveProperties(std::ofstream& ofs) override;
	void LoadProperties(std::ifstream& ifs) override;

public:
	//////////////////////////////////
	////// Initialization Phase //////
	//////////////////////////////////
	virtual void Initialize() override;
	virtual void Setup() override;

	///////////////////////
	////// Game Loop //////
	///////////////////////
	virtual void ProcessInput() override;
	virtual void Update(float deltaTime) override;
	virtual void LateUpdate(float deltaTime) override;
	virtual void Render() override;
	virtual void ProcessEvent() override;

	///////////////////////////////
	////// Termination Phase //////
	///////////////////////////////
	virtual void ShutDown() override;

public:
	D3D11Plugin(const char* name);
	~D3D11Plugin() override;

protected:
	virtual void LoadManagerData(std::ifstream& ifs) override;
	virtual void LoadResourceData(std::ifstream& ifs) override;

private:
	Common::FTDS::DynamicArray<D3D11::D3D11InputDevice*>* mInputDevices;
	D3D11::D3D11Renderer*								  mRenderer;
	Common::FTDS::DynamicArray<D3D11::D3D11Window*>*	  mWindows;
	D3D11::Camera*										  mCamera;

	Common::FTDS::DynamicArray<D3D11::D3D11Component*>* mRegisteredComps;
};

Graphics::ICamera* D3D11Plugin::CreateCamera()
{
	return DBG_NEW D3D11::Camera;
}

InputSystem::IInputDevice* D3D11Plugin::CreateInputDevice()
{
	D3D11::D3D11InputDevice* device = DBG_NEW D3D11::D3D11InputDevice;
	mInputDevices->PushBack(device);
	return device;
}

Graphics::IRenderer* D3D11Plugin::CreateRenderer(Graphics::IWindow* window)
{
	D3D11::D3D11Renderer* renderer = DBG_NEW D3D11::D3D11Renderer(window);
	mRenderer					   = renderer;
	return renderer;
}

void D3D11Plugin::RegisterComponent(Common::IComponent* comp)
{
	mRegisteredComps->PushBack(reinterpret_cast<D3D11::D3D11Component*>(comp));
}

Graphics::IWindow* D3D11Plugin::CreateAppWindow(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea)
{
	D3D11::D3D11Window* window = DBG_NEW D3D11::D3D11Window(title, width, height, rndArea);
	mWindows->PushBack(window);
	return window;
}

Graphics::IWindow* D3D11Plugin::CreateAppWindow(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea, WNDPROC wndProc, void* wndProcParams)
{
	D3D11::D3D11Window* window = 
		DBG_NEW D3D11::D3D11Window(title, width, height, rndArea, wndProc, static_cast<D3D11::WNDPROC_Params*>(wndProcParams));
	mWindows->PushBack(window);
	return window;
}

void D3D11Plugin::Initialize()
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		(*iter)->Initialize();
}

void D3D11Plugin::Setup()
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		(*iter)->Setup();
}

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
		for (auto win = mWindows->Begin(); win != mWindows->End(); ++win)
		{
			(*input)->Update(*win);
		}
	}
}

void D3D11Plugin::Update(float deltaTime)
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->GetIsActive())
			continue;
		(*iter)->Update(deltaTime);
	}
}

void D3D11Plugin::LateUpdate(float deltaTime)
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->GetIsActive())
			continue;
		(*iter)->LateUpdate(deltaTime);
	}
}

void D3D11Plugin::Render()
{
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

void D3D11Plugin::ProcessEvent()
{
}

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

void D3D11Plugin::LoadProperties(std::ifstream& ifs)
{
	LoadManagerData(ifs);
	LoadResourceData(ifs);
}

void D3D11Plugin::ShutDown()
{
}

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

D3D11Plugin::~D3D11Plugin()
{
	delete mRegisteredComps;
	delete mInputDevices;
	delete mRenderer;
	delete mWindows;
}

void D3D11Plugin::LoadManagerData(std::ifstream& ifs)
{
	mCamera->LoadProperties(ifs);
}

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
	D3D11_API Core::IPlugin* CreatePlugin(const char* name)
	{
		return DBG_NEW D3D11::D3D11Plugin(name);
	}

	D3D11_API IComponent* CreateComponent(IPlugin* plugin, IActor* actor, Common::FTDS::String& name)
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