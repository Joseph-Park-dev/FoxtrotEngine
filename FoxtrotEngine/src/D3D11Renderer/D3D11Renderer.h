#pragma once
#include "Core/Renderer/FoxtrotRenderer.h"

class D3D11Renderer :
	public FoxtrotRenderer
{
public:
	void SetViewport(FTVector2 topLeft, FTVector2 resolution);
	void SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY);
	void Reset();
};
