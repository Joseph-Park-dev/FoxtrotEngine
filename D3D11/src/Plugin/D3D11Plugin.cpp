#include "D3D11Plugin.h"

#include "D3D11InputDevice.h"
#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "FTCore.h"
#include "FTDS/Dynamic/DynamicArray.h"

void D3D11Plugin::CreateInputDevice(FTCore* coreInst)
{
	D3D11InputDevice* device = DBG_NEW D3D11InputDevice;
	coreInst->SetInputDevice(device);
	mInputDevices->PushBack(device);
}

void D3D11Plugin::CreateRenderer(FTCore* coreInst)
{
	D3D11Renderer* renderer = DBG_NEW D3D11Renderer;
	coreInst->SetRenderer(renderer);
	mRenderer = renderer;
}

void D3D11Plugin::CretaeWindow(FTCore* coreInst)
{
	D3D11Window* window = DBG_NEW D3D11Window;
	coreInst->SetWindow(window);
	mWindows->PushBack(window);
}

D3D11Plugin::D3D11Plugin(FTResourceDef& resDef)
	: Plugin(resDef)
	, mInputDevices(DBG_NEW FTDS::DynamicArray<D3D11InputDevice*>)
	, mRenderer(DBG_NEW D3D11Renderer)
	, mWindows(DBG_NEW FTDS::DynamicArray<D3D11Window*>)
{
}

D3D11Plugin::~D3D11Plugin()
{
	delete mInputDevices;
	FoxtrotRenderer::Destroy(mRenderer);
	delete mWindows;
}
