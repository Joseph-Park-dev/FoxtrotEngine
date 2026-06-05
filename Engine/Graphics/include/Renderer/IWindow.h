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
namespace Graphics
{
	class IRenderer;
	class IInputDevice;
	class FTRectArea;

	/// <summary>
	/// Abstract window interface defining the contract for all platform windows.
	/// </summary>
	class IWindow
	{
	public:
		//////////////////////////////////////////
		////// Render Frame Lifecycle ////////////
		//////////////////////////////////////////

		/// <summary>
		/// Prepares render targets for a new frame (clears buffers, sets render targets).
		/// </summary>
		/// <param name="renderer">The active renderer instance.</param>
		virtual void BeginRender(IRenderer* renderer) = 0;

		/// <summary>
		/// Finalizes frame rendering (present swap chain, resolve MSAA, etc.).
		/// </summary>
		/// <param name="renderer">The active renderer instance.</param>
		virtual void EndRender(IRenderer* renderer) = 0;

		/// <summary>
		/// Handles window resize events, including buffer recreation.
		/// </summary>
		/// <param name="renderer">The active renderer instance.</param>
		virtual void ResizeWindow(IRenderer* renderer) = 0;

	public:
		//////////////////////////////////////////
		////// Window Properties /////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Returns the window title.
		/// </summary>
		virtual const char* GetTitle() const = 0;

		/// <summary>
		/// Returns the window client width in pixels.
		/// </summary>
		virtual unsigned int GetWidth() const = 0;

		/// <summary>
		/// Returns the window client height in pixels.
		/// </summary>
		virtual unsigned int GetHeight() const = 0;

		/// <summary>
		/// Returns the render area rectangle within the window.
		/// </summary>
		virtual FTRectArea* GetRenderArea() const = 0;

		/// <summary>
		/// Sets the window client width.
		/// </summary>
		virtual void SetWidth(unsigned int width) = 0;

		/// <summary>
		/// Sets the window client height.
		/// </summary>
		virtual void SetHeight(unsigned int height) = 0;

	public:
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