#pragma once
#include "Plugin/Plugin.h"

class D3D11InputDevice;
class D3D11Renderer;
class D3D11Window;

class D3D11Plugin :
	public Plugin
{
public:
	void CreateInputDevice(FTCore* coreInst);
	void CreateRenderer(FTCore* coreInst);
	void CretaeWindow(FTCore* coreInst);

public:
	D3D11Plugin(FTResourceDef& resDef);
	~D3D11Plugin() override;

private:
	FTDS::DynamicArray<D3D11InputDevice*>* mInputDevices;
	D3D11Renderer*						   mRenderer;
	FTDS::DynamicArray<D3D11Window*>*	   mWindows;
};