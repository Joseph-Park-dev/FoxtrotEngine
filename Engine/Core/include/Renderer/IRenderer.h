// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pure virtual interface for all renderer implementations.
/// Platform-specific renderers (D3D11, Vulkan, etc.) must implement this interface.
/// </summary>

#pragma once

#ifdef PLATFORM_D3D11
	#include <d3d11.h>
	#include <wrl.h>
#endif

namespace Math
{
	class FTVector2;
	class FTVector4;
} // namespace Math

namespace Core
{
	class IWindow;

	enum class FillMode
	{
		SOLID,
		WIRE_FRAME
	};

	/// <summary>
	/// Abstract renderer interface defining the contract for all graphics backends.
	/// </summary>
	class IRenderer
	{
	public:
		/// <summary>
		/// Gets the current viewport dimensions.
		/// </summary>
		/// <param name="outTopLeftX">Output: X coordinate of the top-left corner.</param>
		/// <param name="outTopLeftY">Output: Y coordinate of the top-left corner.</param>
		/// <param name="outWidth">Output: Viewport width.</param>
		/// <param name="outHeight">Output: Viewport height.</param>
		virtual void GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const = 0;

		/// <summary>
		/// Sets the viewport for rendering.
		/// </summary>
		/// <param name="topLeftX">X coordinate of the top-left corner.</param>
		/// <param name="topLeftY">Y coordinate of the top-left corner.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		virtual void SetViewport(float topLeftX, float topLeftY, float width, float height) = 0;

#ifdef PLATFORM_D3D11
	public:
		virtual Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice()				 = 0;
		virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext()				 = 0;
		virtual D3D11_VIEWPORT*								 GetViewport()				 = 0;
		virtual const unsigned int							 GetNumQualityLevels() const = 0;
#endif // PLATFORM_D3D11

	protected:
		/// <summary>
		/// Initializes the renderer with the specified window.
		/// </summary>
		/// <param name="window">The window to render to.</param>
		/// <returns>True if initialization succeeded.</returns>
		virtual bool Initialize(IWindow* window) = 0;

		/// <summary>
		/// Resets the renderer state to defaults.
		/// </summary>
		virtual void Reset() = 0;

	public:
		virtual ~IRenderer() = default;
	};
} // namespace Core
