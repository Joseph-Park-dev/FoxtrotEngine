// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTWindow.h"

#include "TemplateFunctions.h"
#include "InputSystem/FTInputDevice.h"
#include "Manager/SceneManager.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Math/FTMath.h"
#include "Renderer/FTRectArea.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui_impl_win32.h>
	#include <imgui_impl_dx11.h>

	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
	#include "DirectoryHelper.h"
	#include "ViewportRenderer.h"
#endif

FTRectArea*	   FTWindow::GetRenderArea() { return mRenderArea; }
FTInputDevice* FTWindow::GetInputDevice() { return mInputDevice; }

FTDS::String& FTWindow::GetTitle()
{
	return mTitle;
}

unsigned int FTWindow::GetWidth() { return mWidth; }
unsigned int FTWindow::GetHeight() { return mHeight; }

void FTWindow::SetWidth(unsigned int width) { mWidth = width; }
void FTWindow::SetHeight(unsigned int height) { mHeight = height; }

/**
 * Determines if a given position lies inside the current render area.
 * Parameters:
 *  - pos: Position in window/client coordinates.
 * Returns:
 *  - true if pos overlaps mRenderArea; false otherwise.
 */
bool FTWindow::IsInRenderedArea(FTVector2 pos)
{
	return mRenderArea->Overlaps(pos);
}

/**
 * Constructor.
 * Parameters:
 *  - title: UTF-8 or ANSI string for window title (converted internally).
 *  - width / height: Desired client dimensions.
 *  - rndArea: Pointer to render area (ownership transferred; deleted in destructor).
 * Behavior:
 *  - Initializes input device.
 * Ownership:
 *  - Assumes rndArea allocated with new; destructor deletes it.
 */
FTWindow::FTWindow(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
	: mWidth(width)
	, mHeight(height)
	, mRenderArea(rndArea)
	, mInputDevice(DBG_NEW FTInputDevice)
{
	mTitle.Assign(title);
}

/**
 * Destructor.
 * - Releases input device and render area (heap-allocated).
 * - Destroys the native window handle (if valid).
 * - COM resources should be previously released (Reset or resize path).
 */
FTWindow::~FTWindow()
{
	delete mInputDevice;
	delete mRenderArea;
}