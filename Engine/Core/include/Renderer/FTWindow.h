// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Entity/Entity.h"

#include "FTDS/Static/FTString.h"

#include "Renderer/FTRectArea.h"

namespace Core
{
	class FoxtrotRenderer;
	class FTInputDevice;
	enum class MOUSE;

	class FTWindow :
		public Core::Entity
	{
	public:
		const bool			  GetIsActive() { return mIsActive; }
		Common::FTDS::String& GetTitle() { return mTitle; }
		unsigned int		  GetWidth() const { return mWidth; };
		unsigned int		  GetHeight() const { return mHeight; }
		FTRectArea*			  GetRenderArea() const { return mRenderArea; };

		/**
		 * @brief Sets the window width (does not automatically resize swap chain).
		 * @param width New width value.
		 */
		void SetWidth(unsigned int width) { mWidth = width; }

		/**
		 * @brief Sets the window height (does not automatically resize swap chain).
		 * @param height New height value.
		 */
		void SetHeight(unsigned int height) { mHeight = height; }

		void SetIsActive(bool val) { mIsActive = val; }

	public:
		/**
		 * @brief Prepares render targets for a new frame (clears, sets RTV/DSV).
		 * @param renderer Active renderer instance.
		 */
		virtual void BeginRender(FoxtrotRenderer* renderer) = 0;

		/**
		 * @brief Finalizes frame rendering (present swap chain, resolve, etc.).
		 * @param renderer Active renderer instance.
		 */
		virtual void EndRender(FoxtrotRenderer* renderer) = 0;

		/**
		 * @brief Handles window resizing logic including swap chain buffer resizing and RTV/DSV recreation.
		 * @param renderer Active renderer instance.
		 */
		virtual void ResizeWindow(FoxtrotRenderer* renderer) = 0;

	public:
		/**
		 * @brief Constructs a window container with title and dimensions.
		 * @param title UTF-8 title string.
		 * @param width Initial window width.
		 * @param height Initial window height.
		 * @param rndArea Pointer to render area definition (may be identical to full window).
		 */
		FTWindow(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
			: mWidth(width)
			, mHeight(height)
			, mRenderArea(rndArea)
			, mIsActive(true)
		{
			mTitle.Assign(title);
		}

		/**
		 * @brief Destructor releases COM resources and associated handles.
		 */
		virtual ~FTWindow()
		{
			delete mRenderArea;
		};

	private:
		bool mIsActive;

		Common::FTDS::String mTitle;	  ///< Window title string.
		unsigned int		 mWidth;	  ///< Current window client width (not render area width).
		unsigned int		 mHeight;	  ///< Current window client height (not render area height).
		FTRectArea*			 mRenderArea; ///< Sub-rectangle defining where the scene is drawn.
	};

	namespace ChunkKey
	{
		namespace FTWindow
		{
			constexpr const char* WINDOW_DATA = "Window Data";
			constexpr const char* WIDTH		  = "Width";
			constexpr const char* HEIGHT	  = "Height";
		} // namespace FTWindow
	} // namespace ChunkKey
} // namespace Core