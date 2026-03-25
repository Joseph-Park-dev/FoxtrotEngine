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