// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton manager that detects the inputs from the player,
/// and their related data. Keyboard clicks and mouse position
/// would be a perfect example.
/// Gamepad input support will be added in the future versions.
/// </summary>

#pragma once
#include "Math/FTMath.h"

namespace FTDS
{
	template <typename TYPE>
	class DynamicArray;
}

enum class KEY
{
	A,
	D,
	W,
	S,
	SHIFT,
	SPACE,
	LAST_FLAG
};

enum class KEY_STATE
{
	NONE,
	TAP,
	HOLD,
	AWAY
};

enum class MOUSE
{
	MOUSE_LEFT,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,

	LAST_FLAG
};

enum class GAMEPADBUTTON
{
	INVALID,
	A,
	B,
	X,
	Y,

	LAST_FLAG
};

struct tKeyInfo
{
	KEY_STATE eKeyState;
	bool	  isPushedPrevFrame;
};

#define MAX_KEYCOUNT 6

class FTInputDevice
{
public:
	void		 DetectMouseDrag(FTVector2& delta);
	virtual void DetectKeyInput() = 0;

public:
	KEY_STATE GetKeyState(KEY eKey);
	KEY_STATE GetMouseState(MOUSE eMouse);
	KEY_STATE GetButtonState(GAMEPADBUTTON eButton);
	FTVector2 GetMousePosition();
	float	  GetMouseWheelDelta();

public:
	bool KEY_HOLD(KEY key);
	bool KEY_TAP(KEY key);
	bool KEY_AWAY(KEY key);
	bool KEY_NONE(KEY key);

	bool MOUSE_HOLD(MOUSE mouse);
	bool MOUSE_TAP(MOUSE mouse);
	bool MOUSE_AWAY(MOUSE mouse);
	bool MOUSE_NONE(MOUSE mouse);

	FTVector2 MOUSE_POS();

public:
	FTInputDevice();
	~FTInputDevice();

protected:
	FTDS::DynamicArray<tKeyInfo>* GetKeyArr() { return mVecKey; }
	FTDS::DynamicArray<tKeyInfo>* GetMouseArr() { return mVecMouse; }
	FTDS::DynamicArray<tKeyInfo>* GetButtonArr() { return mVecButton; }

	int* GetKeyCode() { return mKeyCode; };
	int* GetMouseCode() { return mMouseCode; };

	void SetMousePosition(FTVector2 pos)
	{
		mMousePosX = static_cast<unsigned int>(pos.x);
		mMousePosY = static_cast<unsigned int>(pos.y);
	}

	void SetMousePosition(unsigned int posX, unsigned int posY)
	{
		mMousePosX = posX;
		mMousePosY = posY;
	}

	void SetMouseWheelDelta(float delta)
	{
		mMouseWheelDelta = delta;
	}

private:
	void Init();
};