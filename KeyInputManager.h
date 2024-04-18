#pragma once
#include <vector>
#include <directxtk/Keyboard.h>
#include <directxtk/Mouse.h>

#include "Math.h"
#include "Component.h"
#include "SingletonMacro.h"
#include "unordered_map"

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
	/*void DetectMouseInput();
	void DetectGamepadInput();*/

	//SDL_GameController* FindGamepad();


public:
	KEY_STATE			GetKeyState(DirectX::Keyboard::Keys eKey);
	KEY_STATE			GetMouseState(MOUSE eMouse);
	KEY_STATE			GetButtonState(GAMEPADBUTTON eButton);
	Vector2				GetMousePosition() { return mMousePosition; }
	Vector2				GetMouseWorldPosition();

	/*SDL_GameController* GetGamepad() { return mGamepad; }
	void SetGamepad(SDL_GameController* gamepad) { mGamepad = gamepad; }*/

private:
	std::vector<tKeyInfo>	mVecMouse;
	std::vector<tKeyInfo>	mVecButton;

private:
	using KeyboardMap = std::unordered_map<DirectX::Keyboard::Keys, tKeyInfo>;
	KeyboardMap mKeyboardMap =
	{	
		{DirectX::Keyboard::A,			tKeyInfo{ KEY_STATE::NONE, false }},
		{DirectX::Keyboard::D,			tKeyInfo{ KEY_STATE::NONE, false }},
		{DirectX::Keyboard::W,			tKeyInfo{ KEY_STATE::NONE, false }},
		{DirectX::Keyboard::S,			tKeyInfo{ KEY_STATE::NONE, false }},
		{DirectX::Keyboard::LeftShift,	tKeyInfo{ KEY_STATE::NONE, false }},
		{DirectX::Keyboard::Space,		tKeyInfo{ KEY_STATE::NONE, false }}
	};

	/*int arrMouseCode[(int)MOUSE::LAST_FLAG] =
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
	};*/

private:
	Vector2				mMousePosition;
	int					mMouseState;
	// SDL_GameController* mGamepad;
	DirectX::Keyboard*	mKeyboard;
	DirectX::Mouse*		mMouse;

	DirectX::Keyboard::KeyboardStateTracker* mKeyboardState;

private:
	void Init();
};