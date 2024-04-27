#include "KeyInputManager.h"
#include "Camera2D.h"
#include "TemplateFunctions.h"

KeyInputManager::KeyInputManager()
	: mMousePosition(Vector2::Zero)
	, mMouseState(0)
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

void KeyInputManager::DetectMouseInput(MSG msg)
{
	if (msg.lParam)
	{
		int mouseX = LOWORD(msg.lParam);
		int mouseY = HIWORD(msg.lParam);
		mMousePosition = Vector2((float)mouseX, (float)mouseY);
	}
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
