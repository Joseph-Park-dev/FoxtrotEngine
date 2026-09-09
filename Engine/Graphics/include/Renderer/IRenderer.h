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
namespace Graphics
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
		/// @brief Returns the viewport used by this irenderer.
		/// Gets the current viewport dimensions.
		/// <param name="outTopLeftX">Output: X coordinate of the top-left corner.</param>
		/// <param name="outTopLeftY">Output: Y coordinate of the top-left corner.</param>
		/// <param name="outWidth">Output: Viewport width.</param>
		/// <param name="outHeight">Output: Viewport height.</param>
		/// @param outTopLeftX Receives the viewport's horizontal origin.
		/// @param outTopLeftY Receives the viewport's vertical origin.
		/// @param outWidth Receives the viewport width.
		/// @param outHeight Receives the viewport height.
		virtual void GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const = 0;

		/// @brief Returns how the renderer should fill the inside of the mesh.
		/// @return Current FillMode.
		virtual const Graphics::FillMode& GetFillMode() const = 0;

		/// @brief Updates the viewport used by subsequent operations.
		/// Sets the viewport for rendering.
		/// <param name="topLeftX">X coordinate of the top-left corner.</param>
		/// <param name="topLeftY">Y coordinate of the top-left corner.</param>
		/// <param name="width">Viewport width.</param>
		/// <param name="height">Viewport height.</param>
		/// @param topLeftX Horizontal origin of the viewport or rectangle.
		/// @param topLeftY Vertical origin of the viewport or rectangle.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		virtual void SetViewport(float topLeftX, float topLeftY, float width, float height) = 0;

		// #ifdef PLATFORM_D3D11
		//	public:
		//		virtual Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice()				 = 0;
		//		virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext()				 = 0;
		//		virtual D3D11_VIEWPORT*								 GetViewport()				 = 0;
		//		virtual const unsigned int							 GetNumQualityLevels() const = 0;
		// #endif // PLATFORM_D3D11

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IRenderer() = default;
	};
} // namespace Graphics
