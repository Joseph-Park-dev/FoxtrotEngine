#pragma once
#include "Plugin/Plugin.h"

#include "D3D11InputDevice.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Renderer/FTRectArea.h"

class D3D11InputDevice;
class D3D11Renderer;
class D3D11Window;
class FTRectArea;

#include "Plugin/CoreExports.h"

class D3D11Plugin :
	public Plugin
{
public:
	void CreateInputDevice();
	void CreateRenderer(D3D11Window* window);
	void CreateD3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea);

public:
	D3D11Plugin(FTCore* base, const wchar_t* dllPath);
	~D3D11Plugin();

private:
	FTDS::DynamicArray<D3D11InputDevice*>* mInputDevices;
	D3D11Renderer*						   mRenderer;
	FTDS::DynamicArray<D3D11Window*>*	   mWindows;
};

void D3D11Plugin::CreateInputDevice()
{
	D3D11InputDevice* device = DBG_NEW D3D11InputDevice;
	mInputDevices->PushBack(device);
}

void D3D11Plugin::CreateRenderer(D3D11Window* window)
{
	D3D11Renderer* renderer = DBG_NEW D3D11Renderer(window);
	mRenderer				= renderer;
}

void D3D11Plugin::CreateD3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
{
	D3D11Window* window = DBG_NEW D3D11Window(this, title, width, height, rndArea);
	mWindows->PushBack(window);
}

D3D11Plugin::D3D11Plugin(FTCore* base, const wchar_t* dllPath)
	: Plugin(dllPath)
	, mInputDevices(DBG_NEW FTDS::DynamicArray<D3D11InputDevice*>)
	, mRenderer(nullptr)
	, mWindows(DBG_NEW FTDS::DynamicArray<D3D11Window*>)
{
	FTRectArea* area = DBG_NEW FTRectArea(0.f, 0.f, 500.f, 500.f);
	CreateD3D11Window("Hello!", 500, 500, area);
}

D3D11Plugin::~D3D11Plugin()
{
	delete mInputDevices;
	delete mRenderer;
	delete mWindows;
}

extern "C" CORE_API Plugin* CreatePlugin(FTCore* base, const wchar_t* dllPath)
{
	return new D3D11Plugin(base, dllPath);
}