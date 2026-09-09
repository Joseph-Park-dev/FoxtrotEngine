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
		/// @brief Returns the shared instance used by this manager.
		/// @return Borrowed singleton pointer; do not delete it directly.
		/// @note Synchronize concurrent initialization, access, and destruction externally.
		D3D11_API static DebugShapes* GetInstance();
		/// @brief Deletes the shared instance and clears the singleton pointer.
		/// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
		D3D11_API static void Destroy();
		/// @brief Disables copying so the instance's managed state cannot be duplicated.
		/// @note Unnamed parameter (const DebugShapes&): reserved by this interface or unused by this implementation.
		DebugShapes(const DebugShapes&) = delete;
	protected:
		/// @brief Initializes the debug-geometry registry.
		D3D11_API DebugShapes();
		/// @brief Releases the resources managed by this instance during destruction.
		D3D11_API ~DebugShapes();
	private:
		static DebugShapes* mInstance;
	public:
		// Adds the created shape to the std::vector.
		/// @brief Registers a shape for subsequent rendering.
		/// @param shape Shape to register, remove, or render.
		D3D11_API void AddShape(FTShape* shape);

		// To delete an individual shape, this member function can be called.
		// Do not delete debug shapes manually in Components.
		/// @brief Removes a shape from the managed rendering collection.
		/// @param shape Shape to register, remove, or render.
		D3D11_API void RemoveShape(FTShape* shape);

		// The debug shapes is batch-deleted automatically in this member function.
		// Do not delete debug shapes manually in Components.
		/// @brief Deletes the scene's managed actors and clears its collections.
		D3D11_API void DeleteAll();

	public:
		/// @brief Returns the vs used by this debug shapes.
		/// @return Borrowed access to the vs.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11VertexShader>&	  GetVS();
		/// @brief Returns the gssquare used by this debug shapes.
		/// @return Borrowed access to the gssquare.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetGSSquare();
		/// @brief Returns the ps used by this debug shapes.
		/// @return Borrowed access to the ps.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11PixelShader>&	  GetPS();
		/// @brief Returns the input layout used by this debug shapes.
		/// @return Borrowed access to the input layout.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11InputLayout>&	  GetInputLayout();

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void Initialize(D3D11::D3D11Renderer* renderer);

		// Batch renders shapes.
		// It is not necessary to render shapes manually from the Components
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
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
		/// @brief Creates the shader stages required by this rendering resource.
		/// @param device Direct3D device used to create GPU resources.
		D3D11_API void CreateShaders(Microsoft::WRL::ComPtr<ID3D11Device>& device);
	};
} // namespace D3D11
