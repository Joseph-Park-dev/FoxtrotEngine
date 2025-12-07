// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DebugShapes.h"

#include <wrl.h>

#include "Renderer/D3D11Utils.h"
#include "ResourceSystem/FTRectangle.h"

#ifdef FOXTROT_EDITOR
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

DebugShapes::DebugShapes()
	: mVSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapeVS.hlsl")
	, mGSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapeGS.hlsl")
	, mPSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapePS.hlsl")
	, mCamRect(nullptr)
{
}

DebugShapes::~DebugShapes()
{
	if (0 < mShapes.size())
		mShapes.clear();

#ifdef FOXTROT_EDITOR
	mCamRect = nullptr;
#endif // FOXTROT_EDITOR
}

void DebugShapes::Initialize(FoxtrotRenderer* renderer)
{
	/*if (0 < mShapes.size())
		mShapes.clear();*/
	this->CreateShaders(renderer->GetDevice());
}

void DebugShapes::Render(FoxtrotRenderer* renderer)
{
	if (mShapes.size() < 1)
		return;
	for (FTShape* shape : mShapes)
		shape->Render(renderer);
}

void DebugShapes::AddShape(FTShape* shape)
{
	mShapes.push_back(shape);
}

void DebugShapes::RemoveShape(FTShape* shape)
{
	auto iter = std::find(mShapes.begin(), mShapes.end(), shape);
	if (iter != mShapes.end())
	{
		delete shape;
		shape = nullptr;
		mShapes.erase(iter);
	}
}

void DebugShapes::DeleteAll()
{
	if (mShapes.size() < 1)
		return;
	for (FTShape* shape : mShapes)
	{
		delete shape;
		shape = nullptr;
	}
	mShapes.clear();
}

ComPtr<ID3D11VertexShader>&	  DebugShapes::GetVS() { return mVS; }
ComPtr<ID3D11GeometryShader>& DebugShapes::GetGSSquare() { return mGSSquare; }
ComPtr<ID3D11PixelShader>&	  DebugShapes::GetPS() { return mPS; }
ComPtr<ID3D11InputLayout>&	  DebugShapes::GetInputLayout() { return mInputLayout; }

void DebugShapes::CreateShaders(ComPtr<ID3D11Device>& device)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11Utils::CreateVertexShaderAndInputLayout(
		device, mVSPath, basicInputElements, mVS, mInputLayout);

	D3D11Utils::CreateGeometryShader(
		device, mGSPath, mGSSquare);

	D3D11Utils::CreatePixelShader(
		device, mPSPath, mPS);
}

#ifdef FOXTROT_EDITOR
FTRectangle* DebugShapes::GetCameraRect()
{
	return mCamRect;
}

void DebugShapes::SetCameraRect(FTRectangle* rect)
{
	mCamRect = rect;
}

void DebugShapes::RenderCamRect(FoxtrotRenderer* renderer)
{
	mCamRect->Render(renderer);
}
#endif // FOXTROT_EDITOR
