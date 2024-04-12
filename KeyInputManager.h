#pragma once
#include <SDL2/SDL.h>
#include <vector>

#include "Math.h"
#include "Component.h"
#include "SingletonMacro.h"

enum class KEY_STATE
{
	NONE,
	TAP,
	HOLD,
	AWAY
};

enum class KEY
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	ENTER,
	LSHIFT,
	LCTRL,
	SPACE,

	LAST_FLAG
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
	void DetectMouseInput();
	void DetectGamepadInput();

	SDL_GameController* FindGamepad();

public:
	KEY_STATE			GetKeyState(KEY eKey);
	KEY_STATE			GetMouseState(MOUSE eMouse);
	KEY_STATE			GetButtonState(GAMEPADBUTTON eButton);
	Vector2				GetMousePosition() { return mMousePosition; }
	Vector2				GetMouseWorldPosition();
	SDL_GameController* GetGamepad() { return mGamepad; }

	void SetGamepad(SDL_GameController* gamepad) { mGamepad = gamepad; }

private:
	std::vector<tKeyInfo>mVecKey;
	std::vector<tKeyInfo>mVecMouse;
	std::vector<tKeyInfo>mVecButton;

private:
	SDL_Scancode arrKeyCode[(int)KEY::LAST_FLAG] = 
	{
		SDL_SCANCODE_A,
		SDL_SCANCODE_D,
		SDL_SCANCODE_W,
		SDL_SCANCODE_S,
		SDL_SCANCODE_RETURN,
		SDL_SCANCODE_LSHIFT,
		SDL_SCANCODE_LCTRL,
		SDL_SCANCODE_SPACE
	};
	int arrMouseCode[(int)MOUSE::LAST_FLAG] =
	{
		SDL_BUTTON_LEFT,
		SDL_BUTTON_RIGHT,
		SDL_BUTTON_MIDDLE
	};
	int arrGamepadCode[(int)GAMEPADBUTTON::LAST_FLAG] =
	{
		SDL_CONTROLLER_BUTTON_INVALID,

		SDL_CONTROLLER_BUTTON_A,
		SDL_CONTROLLER_BUTTON_B,
		SDL_CONTROLLER_BUTTON_X,
		SDL_CONTROLLER_BUTTON_Y
	};

private:
	Vector2				mMousePosition;
	int					mMouseState;
	SDL_GameController* mGamepad;

private:
	void Init();
};