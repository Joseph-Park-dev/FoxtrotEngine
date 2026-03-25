// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem/FTInputDevice.h"

#include "TemplateFunctions.h"
#include "FTCore.h"
#include "Renderer/Camera.h"
#include "Renderer/FTWindow.h"
#include "Math/FTMath.h"
#include "Dynamic/DynamicArray.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
#endif // FOXTROT_EDITOR

FTInputDevice::FTInputDevice()
	: mMousePosX(0)
	, mMousePosY(0)
	, mMouseState(0)
	, mMouseWheelDelta(0.f)
	, mIsDragging(false)
{
	mKeyCode = DBG_NEW int[(int)KEY::LAST_FLAG]{
		'A',
		'D',
		'W',
		'S',
		VK_SHIFT,
		VK_SPACE
	};

	mMouseCode = DBG_NEW int[(int)MOUSE::LAST_FLAG]{
		VK_LBUTTON,
		VK_RBUTTON,
		VK_MBUTTON
	};

	mVecKey	   = DBG_NEW	FTDS::DynamicArray<tKeyInfo>();
	mVecMouse  = DBG_NEW  FTDS::DynamicArray<tKeyInfo>();
	mVecButton = DBG_NEW FTDS::DynamicArray<tKeyInfo>();

	Init();
}

FTInputDevice::~FTInputDevice()
{
	delete[] mKeyCode;
	delete[] mMouseCode;
	delete[] mVecKey;
	delete[] mVecMouse;
	delete[] mVecButton;
}

void FTInputDevice::Init()
{
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		mVecKey->PushBack(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)MOUSE::LAST_FLAG; ++i)
	{
		mVecMouse->PushBack(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)GAMEPADBUTTON::LAST_FLAG; ++i)
	{
		mVecButton->PushBack(tKeyInfo{ KEY_STATE::NONE, false });
	}
}

KEY_STATE FTInputDevice::GetKeyState(KEY eKey)
{
	return mVecKey->At((size_t)eKey).eKeyState;
}

KEY_STATE FTInputDevice::GetMouseState(MOUSE eMouse)
{
	return mVecMouse->At((size_t)eMouse).eKeyState;
}

KEY_STATE FTInputDevice::GetButtonState(GAMEPADBUTTON eButton)
{
	return mVecButton->At((size_t)eButton).eKeyState;
}

FTVector2 FTInputDevice::GetMousePosition()
{
	return FTVector2(mMousePosX, mMousePosY);
}

float FTInputDevice::GetMouseWheelDelta()
{
	return mMouseWheelDelta;
}

bool FTInputDevice::KEY_HOLD(KEY key) { return GetKeyState(key) == KEY_STATE::HOLD; }
bool FTInputDevice::KEY_TAP(KEY key) { return GetKeyState(key) == KEY_STATE::TAP; }
bool FTInputDevice::KEY_AWAY(KEY key) { return GetKeyState(key) == KEY_STATE::AWAY; }
bool FTInputDevice::KEY_NONE(KEY key) { return GetKeyState(key) == KEY_STATE::NONE; }

bool FTInputDevice::MOUSE_HOLD(MOUSE mouse) { return GetMouseState(mouse) == KEY_STATE::HOLD; }
bool FTInputDevice::MOUSE_TAP(MOUSE mouse) { return GetMouseState(mouse) == KEY_STATE::TAP; }
bool FTInputDevice::MOUSE_AWAY(MOUSE mouse) { return GetMouseState(mouse) == KEY_STATE::AWAY; }
bool FTInputDevice::MOUSE_NONE(MOUSE mouse) { return GetMouseState(mouse) == KEY_STATE::NONE; }

FTVector2 FTInputDevice::MOUSE_POS() { return FTVector2(mMousePosX, mMousePosY); }

void FTInputDevice::DetectMouseDrag(FTVector2& delta)
{
	static unsigned int mouseX, mouseY;
	if (MOUSE_TAP(MOUSE::MOUSE_LEFT) && !mIsDragging)
	{
		mIsDragging = true;
		mouseX		= mMousePosX;
		mouseY		= mMousePosY;
	}

	if (MOUSE_HOLD(MOUSE::MOUSE_LEFT) && mIsDragging)
	{
		FTVector2 prevPos	 = FTVector2(mouseX, mouseY);
		FTVector2 currentPos = FTVector2(mouseX, mouseY);
		if ((currentPos - prevPos).Length() > 1e-3)
			delta = currentPos - prevPos;
	}

	if (MOUSE_AWAY(MOUSE::MOUSE_LEFT) && mIsDragging)
		mIsDragging = false;
}