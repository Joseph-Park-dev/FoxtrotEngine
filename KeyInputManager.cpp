#include <bitset>
#include <iostream>

#include "KeyInputManager.h"
#include "Camera2D.h"

KeyInputManager::KeyInputManager()
	: mMousePosition(Vector2::Zero)
	, mMouseState(0)
	, mGamepad(nullptr)
{
	Init();
}

KeyInputManager::~KeyInputManager()
{}

void KeyInputManager::Init()
{
	mMousePosition = Vector2::Zero;
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		mVecKey.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)MOUSE::LAST_FLAG; ++i)
	{
		mVecMouse.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
	mGamepad = FindGamepad();
	for (int i = 0; i < (int)GAMEPADBUTTON::LAST_FLAG; ++i)
	{
		mVecButton.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
}

SDL_GameController* KeyInputManager::FindGamepad()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			return SDL_GameControllerOpen(i);
		}
	}
	return nullptr;
}

KEY_STATE KeyInputManager::GetKeyState(KEY eKey)
{
	return mVecKey[(int)eKey].eKeyState;
}

KEY_STATE KeyInputManager::GetMouseState(MOUSE eMouse)
{
	return mVecMouse[(int)eMouse].eKeyState;
}

KEY_STATE KeyInputManager::GetButtonState(GAMEPADBUTTON eButton)
{
	return mVecButton[(int)eButton].eKeyState;
}

Vector2 KeyInputManager::GetMouseWorldPosition()
{
	Vector2 pos = Camera2D::GetInstance()->ConvertScreenPosToWorld(MOUSE_POS);
	return pos;
}

void KeyInputManager::DetectKeyInput()
{
	const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
	for (int i = 0; i < (int)KEY::LAST_FLAG; ++i)
	{
		if (keyboardState[arrKeyCode[i]])
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

void KeyInputManager::DetectMouseInput()
{
	int mouseX = 0;
	int mouseY = 0;
	mMouseState = SDL_GetMouseState(&mouseX, &mouseY);
	mMousePosition = Vector2(mouseX, mouseY);
	for (int mouseButton = 0; mouseButton < (int)MOUSE::LAST_FLAG; ++mouseButton)
	{
		if (mMouseState & (1 << arrMouseCode[mouseButton]-1))
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

void KeyInputManager::DetectGamepadInput()
{
	for (int padButton = 0; padButton < (int)GAMEPADBUTTON::LAST_FLAG; ++padButton)
	{
		const Uint8 buttonState = SDL_GameControllerGetButton(mGamepad, (SDL_GameControllerButton)padButton);
		if (buttonState)
		{
			if (mVecButton[padButton].isPushedPrevFrame)
			{
				mVecButton[padButton].eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				mVecButton[padButton].eKeyState = KEY_STATE::TAP;
			}
			mVecButton[padButton].isPushedPrevFrame = true;
		}
		else
		{
			if (mVecButton[padButton].isPushedPrevFrame)
			{
				mVecButton[padButton].eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				mVecButton[padButton].eKeyState = KEY_STATE::NONE;
			}
			mVecButton[padButton].isPushedPrevFrame = false;
		}
	}
}
