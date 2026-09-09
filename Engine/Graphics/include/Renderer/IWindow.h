// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Pure virtual interface for window implementations.
/// Platform-specific windows must implement this interface.
/// </summary>

#pragma once
namespace D3D11
{
	class FTRectArea;
} // namespace D3D11

namespace Graphics
{
	class IRenderer;
	class IInputDevice;
	using FTRectArea = D3D11::FTRectArea;

	/// <summary>
	/// Abstract window interface defining the contract for all platform windows.
	/// </summary>
	class IWindow
	{
	public:
		//////////////////////////////////////////
		////// Render Frame Lifecycle ////////////
		//////////////////////////////////////////

		/// @brief Binds and clears the render target and configures the viewport for a rendering pass.
		/// Prepares render targets for a new frame (clears buffers, sets render targets).
		/// <param name="renderer">The active renderer instance.</param>
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void BeginRender(IRenderer* renderer) = 0;

		/// @brief Finishes the rendering pass and restores or presents its target.
		/// Finalizes frame rendering (present swap chain, resolve MSAA, etc.).
		/// <param name="renderer">The active renderer instance.</param>
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void EndRender(IRenderer* renderer) = 0;

		/// @brief Updates the window presentation resources to match its client dimensions.
		/// Handles window resize events, including buffer recreation.
		/// <param name="renderer">The active renderer instance.</param>
		/// @param renderer Renderer providing the graphics device and current render state.
		virtual void ResizeWindow(IRenderer* renderer) = 0;

	public:
		//////////////////////////////////////////
		////// Window Properties /////////////////
		//////////////////////////////////////////

		/// @brief Returns the title used by this iwindow.
		/// Returns the window title.
		/// @return Borrowed access to the title.
		virtual const char* GetTitle() const = 0;

		/// @brief Returns the width used by this iwindow.
		/// Returns the window client width in pixels.
		/// @return Current width.
		virtual unsigned int GetWidth() const = 0;

		/// @brief Returns the height used by this iwindow.
		/// Returns the window client height in pixels.
		/// @return Current height.
		virtual unsigned int GetHeight() const = 0;

		/// @brief Returns the render area used by this iwindow.
		/// Returns the render area rectangle within the window.
		/// @return Borrowed access to the render area.
		virtual FTRectArea* GetRenderArea() const = 0;

		/// @brief Updates the width used by subsequent operations.
		/// Sets the window client width.
		/// @param width Replacement width.
		virtual void SetWidth(unsigned int width) = 0;

		/// @brief Updates the height used by subsequent operations.
		/// Sets the window client height.
		/// @param height Replacement height.
		virtual void SetHeight(unsigned int height) = 0;

	public:
		/// @brief Exposes the underlying platform window handle.
		/// @return The underlying platform window handle.
		virtual void* NativeHandle() const = 0;
        /// @brief Completes destruction through the object's inheritance hierarchy.
        virtual ~IWindow() = default;
	};

	namespace ChunkKey
	{
		namespace Window
		{
			constexpr const char* WINDOW_DATA = "Window Data";
			constexpr const char* WIDTH		  = "Width";
			constexpr const char* HEIGHT	  = "Height";
		} // namespace Window
	} // namespace ChunkKey
} // namespace Graphics
