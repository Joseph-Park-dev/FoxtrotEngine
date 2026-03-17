#include "D3D11InputDevice.h"

#include <Windows.h>

#include "Dynamic/DynamicArray.h"
#include "Math/FTMath.h"

void D3D11InputDevice::DetectKeyInput()
{
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		tKeyInfo& key = GetKeyArr()->At(i);
		if (GetAsyncKeyState(GetKeyCode()[i]))
		{
			if (key.isPushedPrevFrame)
			{
				key.eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				key.eKeyState = KEY_STATE::TAP;
			}
			key.isPushedPrevFrame = true;
		}
		else
		{
			if (key.isPushedPrevFrame)
			{
				key.eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				key.eKeyState = KEY_STATE::NONE;
			}
			key.isPushedPrevFrame = false;
		}
	}
}

void D3D11InputDevice::DetectMouseInput(MSG msg)
{
	if (msg.lParam)
	{
		unsigned int mouseX = static_cast<unsigned int>(LOWORD(msg.lParam));
		unsigned int mouseY = static_cast<unsigned int>(HIWORD(msg.lParam));
		SetMousePosition(mouseX, mouseY);
	}

	if (msg.message == WM_MOUSEWHEEL)
		SetMouseWheelDelta(GET_WHEEL_DELTA_WPARAM(msg.wParam));
	else
		SetMouseWheelDelta(0.f);

	for (int i = 0; i < (int)MOUSE::LAST_FLAG; ++i)
	{
		tKeyInfo& mouse = GetMouseArr()->At(i);
		if (GetAsyncKeyState(GetMouseCode()[i]))
		{
			if (mouse.isPushedPrevFrame)
			{
				mouse.eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				mouse.eKeyState = KEY_STATE::TAP;
			}
			mouse.isPushedPrevFrame = true;
		}
		else
		{
			if (mouse.isPushedPrevFrame)
			{
				mouse.eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				mouse.eKeyState = KEY_STATE::NONE;
			}
			mouse.isPushedPrevFrame = false;
		}
	}
}

void D3D11InputDevice::LockCursorInSceneViewport(FTVector2 mousePos)
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

void D3D11InputDevice::UnlockCursorOutOfSceneViewport()
{
	ClipCursor(nullptr);
}