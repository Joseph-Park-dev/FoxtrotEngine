// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Static/FTString.h"

class FoxtrotRenderer;
class FTVector2;
class FTRectArea;
class FTInputDevice;
enum class KEY;
enum class MOUSE;

class FTWindow
{
public:
	FTDS::String& GetTitle() { return mTitle; }
	unsigned int  GetWidth() const { return mWidth; };
	unsigned int  GetHeight() const { return mHeight; }
	FTRectArea*	  GetRenderArea() const { return mRenderArea; };

	/**
	 * @brief Sets the window width (does not automatically resize swap chain).
	 * @param width New width value.
	 */
	void SetWidth(unsigned int width);

	/**
	 * @brief Sets the window height (does not automatically resize swap chain).
	 * @param height New height value.
	 */
	void SetHeight(unsigned int height);

public:
	/**
	 * @brief Polls and processes input events, updating internal FTInputDevice states.
	 * @note Should be called once per frame before querying KEY_/MOUSE_ states.
	 */
	void ProcessInput();

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
	void ResizeWindow(FoxtrotRenderer* renderer);

	/**
	 * @brief Resets and recreates all D3D dependent resources (useful after device changes).
	 * @param renderer Active renderer instance.
	 */

public:
	/**
	 * @brief Constructs a window container with title and dimensions.
	 * @param title UTF-8 title string.
	 * @param width Initial window width.
	 * @param height Initial window height.
	 * @param rndArea Pointer to render area definition (may be identical to full window).
	 */
	FTWindow(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea);

	/**
	 * @brief Destructor releases COM resources and associated handles.
	 */
	virtual ~FTWindow();

private:
	FTDS::String mTitle; ///< Window title string.

	unsigned int mWidth;	  ///< Current window client width (not render area width).
	unsigned int mHeight;	  ///< Current window client height (not render area height).
	FTRectArea*	 mRenderArea; ///< Sub-rectangle defining where the scene is drawn.

private:
	/**
	 * @brief Determines if a position lies within the defined render area.
	 * @param pos Position in window client coordinates.
	 * @return true if inside render area.
	 */
	bool IsInRenderedArea(FTVector2 pos);
};