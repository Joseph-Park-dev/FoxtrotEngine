#pragma once
#include "Plugin/Plugin.h"

#ifdef D3D11RENDERER_EXPORTS
	#define D3D11RENDERER_API __declspec(dllexport)
#else
	#define D3D11RENDERER_API __declspec(dllimport)
#endif

class D3D11InputDevice;
class D3D11Renderer;
class D3D11Window;
class FTRectArea;

class D3D11RENDERER_API D3D11Plugin :
	public Plugin
{
public:
	void CreateInputDevice(FTCore* coreInst);
	void CreateRenderer(FTCore* coreInst, D3D11Window* window);
	void CretaeWindow(FTCore* coreInst, const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea);

public:
	D3D11Plugin(FTResourceDef& resDef);
	~D3D11Plugin() override;

private:
	FTDS::DynamicArray<D3D11InputDevice*>* mInputDevices;
	D3D11Renderer*						   mRenderer;
	FTDS::DynamicArray<D3D11Window*>*	   mWindows;
};