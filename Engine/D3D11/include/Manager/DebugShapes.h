// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton that manages debug shapes for components
/// e.g. Collider2DComponent
/// </summary>

#pragma once
#include "Plugin/D3D11Exports.h"
#include "Entity/Entity.h"

#include <string>
#include <d3d11.h>
#include <wrl.h>

#include "Utility/SingletonMacro.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace D3D11
{
	class FTRectangle;
	class FTShape;
	class D3D11Renderer;

	// This provides a context to render shapes as DebugShapes
	class DebugShapes
	{
	public:
		D3D11_API static DebugShapes* GetInstance();
		D3D11_API static void Destroy();
		DebugShapes(const DebugShapes&) = delete;
	protected:
		D3D11_API DebugShapes();
		D3D11_API ~DebugShapes();
	private:
		static DebugShapes* mInstance;
	public:
		// Adds the created shape to the std::vector.
		D3D11_API void AddShape(FTShape* shape);

		// To delete an individual shape, this member function can be called.
		// Do not delete debug shapes manually in Components.
		D3D11_API void RemoveShape(FTShape* shape);

		// The debug shapes is batch-deleted automatically in this member function.
		// Do not delete debug shapes manually in Components.
		D3D11_API void DeleteAll();

	public:
		D3D11_API Microsoft::WRL::ComPtr<ID3D11VertexShader>&	  GetVS();
		D3D11_API Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetGSSquare();
		D3D11_API Microsoft::WRL::ComPtr<ID3D11PixelShader>&	  GetPS();
		D3D11_API Microsoft::WRL::ComPtr<ID3D11InputLayout>&	  GetInputLayout();

	public:
		D3D11_API void Initialize(D3D11::D3D11Renderer* renderer);

		// Batch renders shapes.
		// It is not necessary to render shapes manually from the Components
		D3D11_API void Render(D3D11::D3D11Renderer* renderer);

	private:
		Common::FTDS::DynamicArray<FTShape*>* mShapes;
		std::wstring						  mVSPath;
		std::wstring						  mGSPath;
		std::wstring						  mPSPath;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	 mVS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGSSquare;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	 mPS;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	 mInputLayout;

	private:
		D3D11_API void CreateShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	};
} // namespace D3D11