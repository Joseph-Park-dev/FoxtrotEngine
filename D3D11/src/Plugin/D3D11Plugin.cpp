#include "D3D11Plugin.h"

#include "D3D11InputDevice.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "FTCore.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Renderer/FTRectArea.h"

void D3D11Plugin::CreateInputDevice(FTCore* coreInst)
{
	D3D11InputDevice* device = DBG_NEW D3D11InputDevice;
	coreInst->SetInputDevice(device);
	mInputDevices->PushBack(device);
}

void D3D11Plugin::CreateRenderer(FTCore* coreInst, D3D11Window* window)
{
	D3D11Renderer* renderer = DBG_NEW D3D11Renderer(window);
	coreInst->SetRenderer(renderer);
	mRenderer = renderer;
}

void D3D11Plugin::CretaeWindow(FTCore* coreInst, const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
{
	D3D11Window* window = DBG_NEW D3D11Window(title, width, height, rndArea);
	coreInst->SetWindow(window);
	mWindows->PushBack(window);
}

D3D11Plugin::D3D11Plugin(FTResourceDef& resDef)
	: Plugin(resDef)
	, mInputDevices(DBG_NEW FTDS::DynamicArray<D3D11InputDevice*>)
	, mRenderer(nullptr)
	, mWindows(DBG_NEW FTDS::DynamicArray<D3D11Window*>)
{
}

D3D11Plugin::~D3D11Plugin()
{
	delete mInputDevices;
	delete mRenderer;
	delete mWindows;
}
