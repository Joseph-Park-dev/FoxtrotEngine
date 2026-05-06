// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/DebugShapes.h"

#include <wrl.h>
#include <d3d11.h>
#include <string>

#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "ResourceSystem/Shape/FTShape.h"
#include "FileSystem/NullKeys.h"
#include "FTDS/Dynamic/DynamicArray.h"

#ifdef FOXTROT_EDITOR
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;
	using Microsoft::WRL::ComPtr;

	DebugShapes::DebugShapes()
		: mShapes(DBG_NEW FTDS::DynamicArray<FTShape*>)
		, mVSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapeVS.hlsl")
		, mGSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapeGS.hlsl")
		, mPSPath(L".\\FoxtrotEngine\\Assets\\Shaders\\DebugShapePS.hlsl")
	{
	}

	DebugShapes::~DebugShapes()
	{
		if (0 < mShapes->GetSize())
			mShapes->Clear();
	}

	void DebugShapes::Initialize(D3D11Renderer* renderer)
	{
		/*if (0 < mShapes.size())
			mShapes.clear();*/
		this->CreateShaders(renderer->GetDevice());
	}

	void DebugShapes::Render(D3D11Renderer* renderer)
	{
		if (mShapes->GetSize() < 1)
			return;

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
			(*iter)->Render(renderer);
	}

	void DebugShapes::AddShape(FTShape* shape)
	{
		mShapes->PushBack(shape);
	}

	void DebugShapes::RemoveShape(FTShape* shape)
	{
		int pos = mShapes->Find(shape);
		if (pos != Core::ChunkKey::NullVal::INVALID_IDX)
		{
			size_t	 idx   = static_cast<size_t>(pos);
			FTShape* shape = mShapes->At(idx);
			delete shape;
			shape = nullptr;
			mShapes->Erase(idx);
		}
	}

	void DebugShapes::DeleteAll()
	{
		if (mShapes->GetSize() < 1)
			return;

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
		{
			delete (*iter);
			(*iter) = nullptr;
		}
		delete mShapes;
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

#endif // FOXTROT_EDITOR
} // namespace D3D11