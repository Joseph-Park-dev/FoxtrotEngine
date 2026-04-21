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
#include "Entity/Entity.h"

#include <string>
#include <d3d11.h>
#include <wrl.h>

#include "SingletonMacro.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace D3D11
{
	class FTRectangle;
	class FTShape;
	class D3D11Renderer;

	// This provides a context to render shapes as DebugShapes
	class DebugShapes :
		public Core::Entity
	{
		SINGLETON_PROTECTED(DebugShapes)
	public:
		// Adds the created shape to the std::vector.
		void AddShape(FTShape* shape);

		// To delete an individual shape, this member function can be called.
		// Do not delete debug shapes manually in Components.
		void RemoveShape(FTShape* shape);

		// The debug shapes is batch-deleted automatically in this member function.
		// Do not delete debug shapes manually in Components.
		void DeleteAll();

	public:
		Microsoft::WRL::ComPtr<ID3D11VertexShader>&	  GetVS();
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetGSSquare();
		Microsoft::WRL::ComPtr<ID3D11PixelShader>&	  GetPS();
		Microsoft::WRL::ComPtr<ID3D11InputLayout>&	  GetInputLayout();

	public:
		void Initialize(D3D11::D3D11Renderer* renderer);

		// Batch renders shapes.
		// It is not necessary to render shapes manually from the Components
		void Render(D3D11::D3D11Renderer* renderer);

	protected:
		void RegisterMemberFuncs() override;

	private:
		Core::FTDS::DynamicArray<FTShape*>* mShapes;
		std::wstring						mVSPath;
		std::wstring						mGSPath;
		std::wstring						mPSPath;

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	 mVS;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mGSSquare;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	 mPS;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	 mInputLayout;

	private:
		void CreateShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	};
} // namespace D3D11