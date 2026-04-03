#pragma once
#include "Plugin/Plugin.h"

#include "InputSystem/D3D11InputDevice.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/Camera.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Renderer/FTRectArea.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/SceneManager.h"
#include "FTCore.h"
#include "Plugin/PluginKey.h"

namespace D3D11
{
	Camera* Camera::mInstance = nullptr;
}

namespace ChunkKey
{
	namespace Plugin
	{
		constexpr const char* D3D11 = "D3D11";
	}
} // namespace ChunkKey

class D3D11Plugin :
	public Plugin
{
public:
	void		 CreateInputDevice();
	void		 CreateRenderer(D3D11::D3D11Window* window);
	void		 CreateD3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea);
	void		 ProcessInput() override;
	virtual void Render(FoxtrotRenderer* renderer) override;

public:
	void SaveProperties() override;
	void LoadProperties(SceneManager* sceneManager);

public:
	D3D11Plugin(FTCore* base);
	~D3D11Plugin() override;

private:
	FTDS::DynamicArray<D3D11::D3D11InputDevice*>* mInputDevices;
	D3D11::D3D11Renderer*						  mRenderer;
	FTDS::DynamicArray<D3D11::D3D11Window*>*	  mWindows;
	D3D11::Camera*								  mCamera;

private:
	void LoadProperties() override;
};

void D3D11Plugin::CreateInputDevice()
{
	D3D11::D3D11InputDevice* device = DBG_NEW D3D11::D3D11InputDevice;
	mInputDevices->PushBack(device);
}

void D3D11Plugin::CreateRenderer(D3D11::D3D11Window* window)
{
	D3D11::D3D11Renderer* renderer = DBG_NEW D3D11::D3D11Renderer(window);
	mRenderer					   = renderer;
}

void D3D11Plugin::CreateD3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
{
	D3D11::D3D11Window* window = DBG_NEW D3D11::D3D11Window(this, title, width, height, rndArea);
	mWindows->PushBack(window);
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
		D3D11::D3D11InputDevice* inp = static_cast<D3D11::D3D11InputDevice*>(*input);
		for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
			(*iter)->ProcessInput(inp);
		Plugin::ProcessInput();
	}
}

void D3D11Plugin::Render(FoxtrotRenderer* renderer)
{
	for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
	{
		// mGameRenderer->RenderClear(mWindow);
		(*iter)->BeginRender(renderer);
		Plugin::Render(renderer);
		(*iter)->EndRender(renderer);
	}
}

void D3D11Plugin::SaveProperties()
{
	FTDS::String dataPath = ChunkKey::Plugin::D3D11;
	dataPath.Append(FileTypes::PLUGIN_DATA);
	std::ofstream ofs(dataPath.C_Str());

	if (ofs.good())
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::Plugin::PLUGIN_DATA);

		mCamera->SaveProperties(ofs);

		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTWindow::WINDOW_DATA);
		for (auto iter = mWindows->Begin(); iter != mWindows->End(); ++iter)
		{
			FileIOHelper::BeginDataPackSave(ofs, (*iter)->GetTitle());
			FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTWindow::WIDTH, (*iter)->GetWidth());
			FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTWindow::HEIGHT, (*iter)->GetHeight());
			gBase->CallFunc<FTRECTAREA_SAVE>(D3D11::PluginKey::D3D11, D3D11::PluginKey::SAVE_PROPERTIES, &ofs, (*iter)->GetRenderArea());
			FileIOHelper::EndDataPackSave(ofs, (*iter)->GetTitle());
		}
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTWindow::WINDOW_DATA);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::Plugin::PLUGIN_DATA);
	}
}

void D3D11Plugin::LoadProperties(SceneManager* sceneManager)
{
	FTDS::String dataPath = ChunkKey::Plugin::D3D11;
	dataPath.Append(FileTypes::PLUGIN_DATA);
	std::ifstream ifs(dataPath.C_Str());
	if (!ifs.good())
		SaveProperties();
	else
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::Plugin::PLUGIN_DATA);
		size_t winCount = FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTWindow::WINDOW_DATA).first;
		for (size_t i = 0; i < winCount; ++i)
		{
			FTDS::String winTitle = FileIOHelper::BeginDataPackLoad(ifs).second;
			FTRectArea*	 rndArea =
				gBase->CallFunc<FTRECTAREA_CONSTRUCTOR, FTRectArea*>(
					D3D11::PluginKey::D3D11,
					D3D11::PluginKey::CREATE_FTRECTAREA);

			unsigned int width	= 0;
			unsigned int height = 0;

			gBase->CallFunc<FTRECTAREA_LOAD>(D3D11::PluginKey::D3D11, D3D11::PluginKey::LOAD_PROPERTIES, &ifs, rndArea);
			FileIOHelper::LoadUnsignedInt(ifs, width);
			FileIOHelper::LoadUnsignedInt(ifs, height);
			CreateD3D11Window(winTitle.C_Str(), width, height, rndArea);
		}

		mCamera->LoadProperties(ifs, sceneManager);
	}
}

D3D11Plugin::D3D11Plugin(FTCore* base)
	: Plugin(base)
	, mInputDevices(DBG_NEW FTDS::DynamicArray<D3D11::D3D11InputDevice*>)
	, mRenderer(nullptr)
	, mWindows(DBG_NEW FTDS::DynamicArray<D3D11::D3D11Window*>)
	, mCamera(nullptr)
{
	FTRectArea* area = DBG_NEW FTRectArea(0.f, 0.f, 500.f, 500.f);
	CreateD3D11Window("Hello!", 500, 500, area);
	CreateInputDevice();
}

D3D11Plugin::~D3D11Plugin()
{
	delete mInputDevices;
	delete mRenderer;
	delete mWindows;
}

void D3D11Plugin::LoadProperties()
{
}

extern "C" __declspec(dllexport) Plugin* CreatePlugin(FTCore* base)
{
	return new D3D11Plugin(base);
}