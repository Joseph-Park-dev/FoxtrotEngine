#include <bitset>
#include <iostream>

#include "KeyInputManager.h"
#include "Camera2D.h"

KeyInputManager::KeyInputManager()
	: mMousePosition(Vector2::Zero)
	, mMouseState(0)
{
	Init();
}

KeyInputManager::~KeyInputManager()
{
	delete mKeyboard;
	delete mKeyboardState;
	delete mMouse;
}

void KeyInputManager::Init()
{
	mKeyboard		= new DirectX::Keyboard;
	mKeyboardState	= new DirectX::Keyboard::KeyboardStateTracker;
	mMouse			= new DirectX::Mouse;

	mMousePosition = Vector2::Zero;
	for (int i = 0; i < (int)MOUSE::LAST_FLAG; ++i)
	{
		mVecMouse.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
	for (int i = 0; i < (int)GAMEPADBUTTON::LAST_FLAG; ++i)
	{
		mVecButton.push_back(tKeyInfo{ KEY_STATE::NONE, false });
	}
}

/*

	Alternative to
	SDL_GameController* KeyInputManager::FindGamepad()
	{
		for (int i = 0; i < SDL_NumJoysticks(); i++) {
			if (SDL_IsGameController(i)) {
				return SDL_GameControllerOpen(i);
			}
		}
		return nullptr;
	}

*/

KEY_STATE KeyInputManager::GetKeyState(DirectX::Keyboard::Keys eKey)
{
	return mKeyboardMap[eKey].eKeyState;
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
	/*KeyboardMap::iterator iter = mKeyboardMap.begin();
	for (; iter != mKeyboardMap.end(); ++iter)
	{
		if (mKeyboardState->IsKeyPressed((*iter).first))
		{
			if ((*iter).second.isPushedPrevFrame)
			{
				(*iter).second.eKeyState = KEY_STATE::HOLD;
			}
			else
			{
				(*iter).second.eKeyState = KEY_STATE::TAP;
			}
			(*iter).second.isPushedPrevFrame = true;
		}
		else if (mKeyboardState->IsKeyReleased((*iter).first)
		{
			if ((*iter).second.isPushedPrevFrame)
			{
				(*iter).second.eKeyState = KEY_STATE::AWAY;
			}
			else
			{
				(*iter).second.eKeyState = KEY_STATE::NONE;
			}
			(*iter).second.isPushedPrevFrame = false;
		}
		else
			return;
	}*/
}

//void KeyInputManager::DetectMouseInput()
//{
//	int mouseX = 0;
//	int mouseY = 0;
//	mMouseState = SDL_GetMouseState(&mouseX, &mouseY);
//	mMousePosition = Vector2(mouseX, mouseY);
//	for (int mouseButton = 0; mouseButton < (int)MOUSE::LAST_FLAG; ++mouseButton)
//	{
//		if (mMouseState & (1 << arrMouseCode[mouseButton]-1))
//		{
//			if (mVecMouse[mouseButton].isPushedPrevFrame)
//			{
//				mVecMouse[mouseButton].eKeyState = KEY_STATE::HOLD;
//			}
//			else
//			{
//				mVecMouse[mouseButton].eKeyState = KEY_STATE::TAP;
//			}
//			mVecMouse[mouseButton].isPushedPrevFrame = true;
//		}
//		else
//		{
//			if (mVecMouse[mouseButton].isPushedPrevFrame)
//			{
//				mVecMouse[mouseButton].eKeyState = KEY_STATE::AWAY;
//			}
//			else
//			{
//				mVecMouse[mouseButton].eKeyState = KEY_STATE::NONE;
//			}
//			mVecMouse[mouseButton].isPushedPrevFrame = false;
//		}
//	}
//}

//void KeyInputManager::DetectGamepadInput()
//{
//	for (int padButton = 0; padButton < (int)GAMEPADBUTTON::LAST_FLAG; ++padButton)
//	{
//		const Uint8 buttonState = SDL_GameControllerGetButton(mGamepad, (SDL_GameControllerButton)padButton);
//		if (buttonState)
//		{
//			if (mVecButton[padButton].isPushedPrevFrame)
//			{
//				mVecButton[padButton].eKeyState = KEY_STATE::HOLD;
//			}
//			else
//			{
//				mVecButton[padButton].eKeyState = KEY_STATE::TAP;
//			}
//			mVecButton[padButton].isPushedPrevFrame = true;
//		}
//		else
//		{
//			if (mVecButton[padButton].isPushedPrevFrame)
//			{
//				mVecButton[padButton].eKeyState = KEY_STATE::AWAY;
//			}
//			else
//			{
//				mVecButton[padButton].eKeyState = KEY_STATE::NONE;
//			}
//			mVecButton[padButton].isPushedPrevFrame = false;
//		}
//	}
//}
