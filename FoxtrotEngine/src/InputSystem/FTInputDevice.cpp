// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "InputSystem/FTInputDevice.h"

#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "Renderer/Camera.h"
#include "WindowSystem/FTWindow.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
#endif // FOXTROT_EDITOR

FTInputDevice::FTInputDevice()
	: mMousePosition(FTVector2::Zero)
	, mMouseState(0)
	, mMouseWheelDelta(0.f)
	, mIsDragging(false)
{
	Init();
}

FTInputDevice::~FTInputDevice()
{
}

void FTInputDevice::Init()
{
	mMousePosition = FTVector2::Zero;
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		mVecKey.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)MOUSE::LAST_FLAG; ++i)
	{
		mVecMouse.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)GAMEPADBUTTON::LAST_FLAG; ++i)
	{
		mVecButton.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
}

KEY_STATE FTInputDevice::GetKeyState(KEY eKey)
{
	return mVecKey[(int)eKey].eKeyState;
}

KEY_STATE FTInputDevice::GetMouseState(MOUSE eMouse)
{
	return mVecMouse[(int)eMouse].eKeyState;
}

KEY_STATE FTInputDevice::GetButtonState(GAMEPADBUTTON eButton)
{
	return mVecButton[(int)eButton].eKeyState;
}

FTVector2 FTInputDevice::GetMousePosition()
{
	return mMousePosition;
}

FTVector2 FTInputDevice::GetMouseWorldPosition()
{
	return Camera::GetInstance()->ConvertScreenPosToWorld(mMousePosition);
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

FTVector2 FTInputDevice::MOUSE_POS() { return mMousePosition; }
FTVector3 FTInputDevice::MOUSE_WORLDPOS(Camera* camInst) { return camInst->ConvertScreenPosToWorld(mMousePosition); }

void FTInputDevice::DetectKeyInput()
{
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		if (GetAsyncKeyState(mKeyCode[i]))
		{
			if (mVecKey[i].isPushedPrevFrame)
			{
				mVecKey[i].eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				mVecKey[i].eKeyState = KEY_STATE::TAP;
			}
			mVecKey[i].isPushedPrevFrame = true;
		}
		else
		{
			if (mVecKey[i].isPushedPrevFrame)
			{
				mVecKey[i].eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				mVecKey[i].eKeyState = KEY_STATE::NONE;
			}
			mVecKey[i].isPushedPrevFrame = false;
		}
	}
}

void FTInputDevice::DetectMouseInput(MSG msg)
{
	if (msg.lParam)
	{
		int mouseX	   = LOWORD(msg.lParam);
		int mouseY	   = HIWORD(msg.lParam);
		mMousePosition = FTVector2((float)mouseX, (float)mouseY);

//#ifdef FOXTROT_EDITOR
//		ImVec2 viewportPos = EditorLayer::GetInstance()->GetSceneViewportPos();
//		mMousePosition -= viewportPos;
//
//#endif // FOXTROT_EDITOR
	}

	if (msg.message == WM_MOUSEWHEEL)
		mMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
	else
		mMouseWheelDelta = 0.f;

	for (int mouseButton = 0; mouseButton < (int)MOUSE::LAST_FLAG; ++mouseButton)
	{
		if (GetAsyncKeyState(mMouseCode[mouseButton]))
		{
			if (mVecMouse[mouseButton].isPushedPrevFrame)
			{
				mVecMouse[mouseButton].eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				mVecMouse[mouseButton].eKeyState = KEY_STATE::TAP;
			}
			mVecMouse[mouseButton].isPushedPrevFrame = true;
		}
		else
		{
			if (mVecMouse[mouseButton].isPushedPrevFrame)
			{
				mVecMouse[mouseButton].eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				mVecMouse[mouseButton].eKeyState = KEY_STATE::NONE;
			}
			mVecMouse[mouseButton].isPushedPrevFrame = false;
		}
	}
}

void FTInputDevice::DetectMouseDrag(FTVector3& delta)
{
	static FTVector3 prevPos;
	if (MOUSE_TAP(MOUSE::MOUSE_LEFT) && !mIsDragging)
	{
		mIsDragging = true;
		prevPos		= mMousePosition;
	}

	if (MOUSE_HOLD(MOUSE::MOUSE_LEFT) && mIsDragging)
	{
		FTVector3 currentPos = mMousePosition;
		if ((currentPos - prevPos).Length() > 1e-3)
		{
			delta = currentPos - prevPos;
			printf("%f %f %f \n", currentPos.x, currentPos.y, currentPos.z);
		}
	}

	if (MOUSE_AWAY(MOUSE::MOUSE_LEFT) && mIsDragging)
		mIsDragging = false;
}

void FTInputDevice::LockCursorInSceneViewport(FTVector2 mousePos)
{
	RECT rect;
	GetClientRect(FTCore::GetInstance()->GetGameWindow()->GetHandle(), &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	MapWindowPoints(FTCore::GetInstance()->GetGameWindow()->GetHandle(), nullptr, &ul, 1);
	MapWindowPoints(FTCore::GetInstance()->GetGameWindow()->GetHandle(), nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top  = ul.y;

	rect.right	= lr.x;
	rect.bottom = lr.y;
	ClipCursor(&rect);
}

void FTInputDevice::UnlockCursorOutOfSceneViewport()
{
	ClipCursor(nullptr);
}