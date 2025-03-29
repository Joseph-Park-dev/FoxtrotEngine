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
#include <unordered_map>
#include <vector>
#include <queue>

#include <Windows.h>

#include "Math/FTMath.h"

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
	void DetectKeyInput();
	void DetectMouseInput(MSG msg);
	void LockCursorInSceneViewport(FTVector2 mousePos);
	void UnlockCursorOutOfSceneViewport();
	// void DetectGamepadInput();

	// SDL_GameController* FindGamepad();
public:
	KEY_STATE GetKeyState(KEY eKey);
	KEY_STATE GetMouseState(MOUSE eMouse);
	KEY_STATE GetButtonState(GAMEPADBUTTON eButton);
	FTVector2 GetMousePosition();
	FTVector2 GetMouseWorldPosition();
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

private:
	// Keyboard related data.
	std::vector<tKeyInfo> mVecKey;
	std::vector<tKeyInfo> mVecMouse;
	std::vector<tKeyInfo> mVecButton;

	// Mouse related data.
	FTVector2 mMousePosition;
	int		  mMouseState;
	float	  mMouseWheelDelta;

private:
	using KeyboardMap				  = std::unordered_map<unsigned char, tKeyInfo>;
	int mKeyCode[(int)KEY::LAST_FLAG] = {
		'A',
		'D',
		'W',
		'S',
		VK_SHIFT,
		VK_SPACE
	};

	int mMouseCode[(int)MOUSE::LAST_FLAG] = {
		VK_LBUTTON,
		VK_RBUTTON,
		VK_MBUTTON
	};

private:
	void Init();
};