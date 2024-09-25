#pragma once
#include <unordered_map>
#include <vector>
#include <queue>

#include <Windows.h>

#include "Core/SingletonMacro.h"

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

#define KEY_HOLD(key)	  KeyInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::HOLD
#define KEY_TAP(key)	  KeyInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::TAP
#define KEY_AWAY(key)	  KeyInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::AWAY
#define KEY_NONE(key)	  KeyInputManager::GetInstance()->GetKeyState(key) == KEY_STATE::NONE

#define MOUSE_HOLD(key)	  KeyInputManager::GetInstance()->GetMouseState(key) == KEY_STATE::HOLD
#define MOUSE_TAP(key)	  KeyInputManager::GetInstance()->GetMouseState(key) == KEY_STATE::TAP
#define MOUSE_AWAY(key)	  KeyInputManager::GetInstance()->GetMouseState(key) == KEY_STATE::AWAY
#define MOUSE_NONE(key)	  KeyInputManager::GetInstance()->GetMouseState(key) == KEY_STATE::NONE
						  
#define BUTTON_HOLD(key)  KeyInputManager::GetInstance()->GetButtonState(key) == KEY_STATE::HOLD
#define BUTTON_TAP(key)	  KeyInputManager::GetInstance()->GetButtonState(key) == KEY_STATE::TAP
#define BUTTON_AWAY(key)  KeyInputManager::GetInstance()->GetButtonState(key) == KEY_STATE::AWAY
#define BUTTON_NONE(key)  KeyInputManager::GetInstance()->GetButtonState(key) == KEY_STATE::NONE
						  
// Returns Mouse World Pos 
#define MOUSE_POS		  KeyInputManager::GetInstance()->GetMousePosition()
#define MOUSE_WORLDPOS	  KeyInputManager::GetInstance()->GetMouseWorldPosition()

class KeyInputManager
{
	SINGLETON(KeyInputManager);

public:
	void DetectKeyInput();
	void DetectMouseInput(MSG msg);
	void LockCursorInSceneViewport(FTVector2 mousePos);
	void UnlockCursorOutOfSceneViewport();
	//void DetectGamepadInput();

	//SDL_GameController* FindGamepad();
public:
	KEY_STATE	GetKeyState(KEY eKey);
	KEY_STATE	GetMouseState(MOUSE eMouse);
	KEY_STATE	GetButtonState(GAMEPADBUTTON eButton);
	FTVector2	GetMousePosition() { return mMousePosition; }
	FTVector2	GetMouseWorldPosition();

	/*SDL_GameController* GetGamepad() { return mGamepad; }
	void SetGamepad(SDL_GameController* gamepad) { mGamepad = gamepad; }*/

private:
	std::vector<tKeyInfo>	mVecKey;
	std::vector<tKeyInfo>	mVecMouse;
	std::vector<tKeyInfo>	mVecButton;

private:
	using KeyboardMap = std::unordered_map<unsigned char, tKeyInfo>;
	int mKeyCode[(int)KEY::LAST_FLAG] =
	{
		'A',
		'D',
		'W',
		'S',
		VK_SHIFT,
		VK_SPACE
	};

	int mMouseCode[(int)MOUSE::LAST_FLAG] =
	{
		VK_LBUTTON,
		VK_RBUTTON,
		VK_MBUTTON
	};
	/*int arrGamepadCode[(int)GAMEPADBUTTON::LAST_FLAG] =
	{
		SDL_CONTROLLER_BUTTON_INVALID,

		SDL_CONTROLLER_BUTTON_A,
		SDL_CONTROLLER_BUTTON_B,
		SDL_CONTROLLER_BUTTON_X,
		SDL_CONTROLLER_BUTTON_Y
	};*/

private:
	FTVector2			mMousePosition;
	int					mMouseState;

private:
	void Init();
};