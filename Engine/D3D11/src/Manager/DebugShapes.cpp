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

#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Shape/FTRectangle.h"
#include "ResourceSystem/Shape/FTShape.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTGeometryShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "FileSystem/NullKeys.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Manager/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using Microsoft::WRL::ComPtr;

	/// @brief Initializes the debug-geometry registry.
	/// @note Initializes the :DebugShapes base or delegates to its constructor.
	DebugShapes::DebugShapes()
		: mShapes(DBG_NEW Common::FTDS::DynamicArray<FTShape*>)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	DebugShapes::~DebugShapes()
	{
		DeleteAll();
		delete mShapes;
	}

	D3D11_API FTRectangle* DebugShapes::AddRectangle()
	{
		FTRectangle* rect = DBG_NEW FTRectangle;
		AddShape(rect);
		return rect;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void DebugShapes::Initialize()
	{
		ResourceManager* resources = ResourceManager::GetInstance();
		if (!resources)
			return;

		FTVertexShader*	  vertexShader	 = resources->GetVertexShader("DebugShapeVS.hlsl");
		FTGeometryShader* geometryShader = resources->GetGeometryShader("DebugShapeGS.hlsl");
		FTPixelShader*	  pixelShader	 = resources->GetPixelShader("DebugShapePS.hlsl");
		if (!vertexShader || !geometryShader || !pixelShader)
			return;

		mVS			 = vertexShader->GetShader();
		mInputLayout = vertexShader->GetInputLayout();
		mGSSquare	 = geometryShader->GetShader();
		mPS			 = pixelShader->GetShader();
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	void DebugShapes::Render(D3D11Renderer* renderer)
	{
		if (!renderer || !mShapes || mShapes->GetSize() < 1)
			return;

		if (!mVS || !mGSSquare || !mPS || !mInputLayout)
			Initialize();
		if (!mVS || !mGSSquare || !mPS || !mInputLayout)
			return;

		// Scene and ImGui draws leave scissor, depth, and blend state that clip or discard the outline.
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
		context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
		context->OMSetDepthStencilState(nullptr, 0);

		static Microsoft::WRL::ComPtr<ID3D11RasterizerState> debugRasterizer;
		if (!debugRasterizer)
		{
			D3D11_RASTERIZER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.FillMode			   = D3D11_FILL_SOLID;
			desc.CullMode			   = D3D11_CULL_NONE;
			desc.DepthClipEnable	   = TRUE;
			desc.ScissorEnable		   = FALSE;
			desc.AntialiasedLineEnable = TRUE;
			renderer->GetDevice()->CreateRasterizerState(&desc, debugRasterizer.GetAddressOf());
		}
		if (debugRasterizer)
			context->RSSetState(debugRasterizer.Get());

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
			(*iter)->Render(renderer);
	}

	/// @brief Registers a shape for subsequent rendering.
	/// @param shape Shape to register, remove, or render.
	void DebugShapes::AddShape(FTShape* shape)
	{
		mShapes->PushBack(shape);
	}

	/// @brief Removes a shape from the managed rendering collection.
	/// @param shape Shape to register, remove, or render.
	void DebugShapes::RemoveShape(FTShape* shape)
	{
		int pos = mShapes->Find(shape);
		if (pos != Common::ChunkKey::NullVal::INVALID_IDX)
		{
			size_t	 idx   = static_cast<size_t>(pos);
			FTShape* shape = mShapes->At(idx);
			delete shape;
			shape = nullptr;
			mShapes->Erase(idx);
		}
	}

	/// @brief Deletes the scene's managed actors and clears its collections.
	void DebugShapes::DeleteAll()
	{
		if (mShapes->GetSize() < 1)
			return;

		for (auto iter = mShapes->Begin(); iter != mShapes->End(); ++iter)
		{
			delete (*iter);
			(*iter) = nullptr;
		}
		mShapes->Clear();
	}

	/// @brief Returns the vs used by this debug shapes.
	/// @return Borrowed access to the vs.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11VertexShader>& DebugShapes::GetVS() { return mVS; }
	/// @brief Returns the gssquare used by this debug shapes.
	/// @return Borrowed access to the gssquare.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11GeometryShader>& DebugShapes::GetGSSquare() { return mGSSquare; }
	/// @brief Returns the ps used by this debug shapes.
	/// @return Borrowed access to the ps.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11PixelShader>& DebugShapes::GetPS() { return mPS; }
	/// @brief Returns the input layout used by this debug shapes.
	/// @return Borrowed access to the input layout.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11InputLayout>& DebugShapes::GetInputLayout() { return mInputLayout; }

	/// @brief Creates the shader stages required by this rendering resource.
	/// @param device Direct3D device used to create GPU resources.
	void DebugShapes::CreateShaders(ComPtr<ID3D11Device>& device)
	{
		// std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// };
	}

#ifdef FOXTROT_EDITOR

#endif // FOXTROT_EDITOR
} // namespace D3D11
namespace D3D11
{
	DebugShapes* DebugShapes::mInstance = nullptr;

	/// @brief Returns the shared instance used by this manager.
	/// @return Borrowed singleton pointer; do not delete it directly.
	/// @note Synchronize concurrent initialization, access, and destruction externally.
	DebugShapes* DebugShapes::GetInstance()
	{
		if (!mInstance)
			mInstance = DBG_NEW DebugShapes();
		return mInstance;
	}

	/// @brief Deletes the shared instance and clears the singleton pointer.
	/// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
	void DebugShapes::Destroy()
	{
		delete mInstance;
		mInstance = nullptr;
	}
} // namespace D3D11
