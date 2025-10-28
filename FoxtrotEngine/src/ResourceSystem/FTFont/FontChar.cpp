#include "FontChar.h"

#include <d3d11.h>

#include "ResourceSystem/FTFont/TextVertex.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTFont/TextVertex.h"


FontChar::FontChar(FontCharData& data)
	: mID(data.ID)
	, mU(data.U)
	, mV(data.V)
	, mWidthOnTex(data.WidthOnTex)
	, mHeightOnTex(data.HeightOnTex)
	, mWidthOnScreen(data.WidthOnScreen)
	, mHeightOnScreen(data.HeightOnScreen)
	, mOffsetX(data.OffsetX)
	, mOffsetY(data.OffsetY)
	, mAdvanceX(data.AdvanceX)
{
}

int FontChar::ID()
{
	return mID;
}

float FontChar::U()
{
	return mU;
}

float FontChar::V()
{
	return mV;
}

float FontChar::WidthOnTex()
{
	return mWidthOnTex;
}

float FontChar::HeightOnTex()
{
	return mHeightOnTex;
}

float FontChar::WidthOnScreen()
{
	return mWidthOnScreen;
}

float FontChar::HeightOnScreen()
{
	return mHeightOnScreen;
}

float FontChar::OffsetX()
{
	return mOffsetX;
}

float FontChar::OffsetY()
{
	return mOffsetY;
}

float FontChar::AdvanceX()
{
	return mAdvanceX;
}
