#pragma once
#include "Plugin/IPlugin.h"
#include "Factory/IGraphicsFactory.h"
#include "Factory/IInputSysFactory.h"

#include "Actor/IActor.h"
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

namespace ChunkKey
{
	namespace Plugin
	{
		constexpr const char* D3D11 = "D3D11";
	}
} // namespace ChunkKey

using namespace Core;
using namespace Common;
class D3D11Plugin :
	public Core::IPlugin,
	public Core::IGraphicsFactory,
	public Core::IInputSysFactory
{
public:
	virtual IWindow* CreateAppWindow(
		const char*	 title,
		unsigned int width,
		unsigned int height,
		FTRectArea*	 renderArea) override;
	virtual IRenderer*	  CreateRenderer(IWindow* window) override;
	virtual ICamera*	  CreateCamera() override;
	virtual IInputDevice* CreateInputDevice() override;

	void SaveProperties();
	void LoadProperties(SceneManager* sceneManager);

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

private:
	Common::FTDS::DynamicArray<D3D11::D3D11InputDevice*>* mInputDevices;
	D3D11::D3D11Renderer*								  mRenderer;
	Common::FTDS::DynamicArray<D3D11::D3D11Window*>*	  mWindows;
	D3D11::Camera*										  mCamera;

	Common::FTDS::DynamicArray<D3D11::D3D11Component*>* mRegisteredComps;
};

ICamera* D3D11Plugin::CreateCamera()
{
	return D3D11::Camera::GetInstance();
}

IInputDevice* D3D11Plugin::CreateInputDevice()
{
	D3D11::D3D11InputDevice* device = DBG_NEW D3D11::D3D11InputDevice;
	mInputDevices->PushBack(device);
	return device;
}

IRenderer* D3D11Plugin::CreateRenderer(IWindow* window)
{
	D3D11::D3D11Renderer* renderer = DBG_NEW D3D11::D3D11Renderer(window);
	mRenderer					   = renderer;
	return renderer;
}

IWindow* D3D11Plugin::CreateAppWindow(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
{
	D3D11::D3D11Window* window = DBG_NEW D3D11::D3D11Window(title, width, height, rndArea);
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
		if (!(*iter)->GetOwner()->IsActive())
			continue;
		(*iter)->Update(deltaTime);
	}
}

void D3D11Plugin::LateUpdate(float deltaTime)
{
	for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->IsActive())
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

void D3D11Plugin::SaveProperties()
{
	// Common::FTDS::String dataPath = D3D11::PluginKey::D3D11;
	// dataPath.Append(FileTypes::PLUGIN_DATA);
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

	//		HMODULE			mod	 = GetModuleHandleA(DLLPaths::CORE_EDITOR);
	//		FARPROC			proc = GetProcAddress(mod, D3D11::PluginKey::SAVE_PROPERTIES);
	//		FTRECTAREA_SAVE func = reinterpret_cast<FTRECTAREA_SAVE>(proc);
	//		func(ofs, (*iter)->GetRenderArea());
	//		FileIOHelper::EndDataPackSave(ofs, (*iter)->GetTitle());
	//	}
	//	FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::FTWindow::WINDOW_DATA);

	//	FileIOHelper::EndDataPackSave(ofs, Core::ChunkKey::Plugin::PLUGIN_DATA);
	//}
}

void D3D11Plugin::LoadProperties(SceneManager* sceneManager)
{
	/*Common::FTDS::String dataPath = D3D11::PluginKey::D3D11;
	dataPath.Append(FileTypes::PLUGIN_DATA);
	std::ifstream ifs(dataPath.C_Str());
	if (!ifs.good())
		SaveProperties();
	else
	{
		FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::Plugin::PLUGIN_DATA);
		size_t winCount = FileIOHelper::BeginDataPackLoad(ifs, Core::ChunkKey::FTWindow::WINDOW_DATA).first;
		for (size_t i = 0; i < winCount; ++i)
		{
			Common::FTDS::String   winTitle = FileIOHelper::BeginDataPackLoad(ifs).second;
			HMODULE				   mod		= GetModuleHandleA(DLLPaths::CORE_EDITOR);
			FARPROC				   proc		= GetProcAddress(mod, D3D11::PluginKey::CREATE_FTRECTAREA);
			FTRECTAREA_CONSTRUCTOR func		= reinterpret_cast<FTRECTAREA_CONSTRUCTOR>(proc);

			FTRectArea*	 rndArea = func(0.f, 0.f, 0.f, 0.f, 0.f);
			unsigned int width	 = 0;
			unsigned int height	 = 0;

			proc					 = GetProcAddress(mod, D3D11::PluginKey::LOAD_PROPERTIES);
			FTRECTAREA_LOAD loadFunc = reinterpret_cast<FTRECTAREA_LOAD>(proc);
			loadFunc(ifs, rndArea);

			FileIOHelper::LoadUnsignedInt(ifs, width);
			FileIOHelper::LoadUnsignedInt(ifs, height);
			CreateD3D11Window(winTitle.C_Str(), width, height, rndArea);
		}

		mCamera->LoadProperties(ifs, sceneManager);
	}*/
}

void D3D11Plugin::ShutDown()
{
}

D3D11Plugin::D3D11Plugin(const char* name)
	: mInputDevices(DBG_NEW Common::FTDS::DynamicArray<D3D11::D3D11InputDevice*>)
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
	delete mInputDevices;
	delete mRenderer;
	delete mWindows;
}

extern "C" __declspec(dllexport) Core::IPlugin* CreatePlugin(const char* name)
{
	return new D3D11Plugin(name);
}