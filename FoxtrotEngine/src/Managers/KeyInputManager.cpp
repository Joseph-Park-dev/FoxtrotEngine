#include "Managers/KeyInputManager.h"

#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

#ifdef FOXTROT_EDITOR
#include "EditorLayer.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif // FOXTROT_EDITOR

KeyInputManager::KeyInputManager()
	: mMousePosition(FTVector2::Zero)
	, mMouseState(0)
{
	Init();
}

KeyInputManager::~KeyInputManager()
{}

void KeyInputManager::Init()
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

FTVector2 KeyInputManager::GetMouseWorldPosition()
{
	return FTVector2::Zero;
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
		mMousePosition = FTVector2((float)mouseX, (float)mouseY);
#ifdef FOXTROT_EDITOR
		ImVec2 viewportPos = EditorLayer::GetInstance()->GetSceneViewportPos();
		mMousePosition -= viewportPos;

#endif // FOXTROT_EDITOR
		//mMousePosition *= Camera::GetInstance()->GetNDCRatio();
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

void KeyInputManager::LockCursorInSceneViewport(FTVector2 mousePos)
{
	RECT rect;
	GetClientRect(FTCore::GetInstance()->GetWindow(), &rect);

	POINT ul;
	ul.x = rect.left;
	ul.y = rect.top;

	POINT lr;
	lr.x = rect.right;
	lr.y = rect.bottom;

	MapWindowPoints(FTCore::GetInstance()->GetWindow(), nullptr, &ul, 1);
	MapWindowPoints(FTCore::GetInstance()->GetWindow(), nullptr, &lr, 1);

	rect.left = ul.x;
	rect.top = ul.y;

	rect.right = lr.x;
	rect.bottom = lr.y;
	ClipCursor(&rect);
}

void KeyInputManager::UnlockCursorOutOfSceneViewport()
{
	ClipCursor(nullptr);
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
