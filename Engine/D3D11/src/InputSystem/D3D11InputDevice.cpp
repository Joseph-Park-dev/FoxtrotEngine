#include "InputSystem/D3D11InputDevice.h"

#include <Windows.h>

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTMath.h"
#include "Debugging/DebugMemAlloc.h"
#include "Renderer/D3D11Window.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	void D3D11InputDevice::DetectKeyboardInput()
	{
		for (size_t i = 0; i < (size_t)KEYBOARD::LAST_FLAG; ++i)
		{
			ButtonInput& btnInput = GetButtonInput(mKeyboardButtons, i);
			if (GetAsyncKeyState(mKeyboardCode[i]))
			{
				if (btnInput.IsPushedPrevFrame)
				{
					btnInput.ButtonState = ButtonState::HOLD;
				}
				else
				{
					btnInput.ButtonState = ButtonState::TAP;
				}
				btnInput.IsPushedPrevFrame = true;
			}
			else
			{
				if (btnInput.IsPushedPrevFrame)
				{
					btnInput.ButtonState = ButtonState::AWAY;
				}
				else
				{
					btnInput.ButtonState = ButtonState::NONE;
				}
				btnInput.IsPushedPrevFrame = false;
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

		for (size_t i = 0; i < (size_t)MOUSE::LAST_FLAG; ++i)
		{
			ButtonInput& mouse = GetButtonInput(mMouseButtons, i);
			if (GetAsyncKeyState(mMouseCode[i]))
			{
				if (mouse.IsPushedPrevFrame)
				{
					mouse.ButtonState = ButtonState::HOLD;
				}
				else
				{
					mouse.ButtonState = ButtonState::TAP;
				}
				mouse.IsPushedPrevFrame = true;
			}
			else
			{
				if (mouse.IsPushedPrevFrame)
				{
					mouse.ButtonState = ButtonState::AWAY;
				}
				else
				{
					mouse.ButtonState = ButtonState::NONE;
				}
				mouse.IsPushedPrevFrame = false;
			}
		}
	}

	void D3D11InputDevice::DetectMouseDrag(int& deltaX, int& deltaY)
	{
		static int mouseX, mouseY;
		if (MOUSE_TAP(MOUSE::MOUSE_LEFT) && !mIsDragging)
		{
			mIsDragging = true;
			mouseX		= static_cast<int>(mMousePosX);
			mouseY		= static_cast<int>(mMousePosY);
		}

		if (MOUSE_HOLD(MOUSE::MOUSE_LEFT) && mIsDragging)
		{
			deltaX = static_cast<int>(mMousePosX) - mouseX;
			deltaY = static_cast<int>(mMousePosY) - mouseY;
		}

		if (MOUSE_AWAY(MOUSE::MOUSE_LEFT) && mIsDragging)
			mIsDragging = false;
	}

	bool D3D11InputDevice::KEY_HOLD(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::HOLD; }
	bool D3D11InputDevice::KEY_TAP(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::TAP; }
	bool D3D11InputDevice::KEY_AWAY(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::AWAY; }
	bool D3D11InputDevice::KEY_NONE(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::NONE; }

	bool D3D11InputDevice::MOUSE_HOLD(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::HOLD; }
	bool D3D11InputDevice::MOUSE_TAP(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::TAP; }
	bool D3D11InputDevice::MOUSE_AWAY(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::AWAY; }
	bool D3D11InputDevice::MOUSE_NONE(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::NONE; }

	unsigned int D3D11InputDevice::MOUSE_X() { return mMousePosX; }
	unsigned int D3D11InputDevice::MOUSE_Y() { return mMousePosY; }

	void D3D11InputDevice::LockCursorInSceneViewport(D3D11Window* window, Math::FTVector2 mousePos)
	{
		RECT rect;
		GetClientRect(window->GetHandle(), &rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints(window->GetHandle(), nullptr, &ul, 1);
		MapWindowPoints(window->GetHandle(), nullptr, &lr, 1);

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

	const unsigned int D3D11InputDevice::GetMousePosX() const
	{
		return mMousePosX;
	}

	const unsigned int D3D11InputDevice::GetMousePosY() const
	{
		return mMousePosY;
	}

	const float D3D11InputDevice::GetMouseWheelDelta() const
	{
		return mMouseWheelDelta;
	}

	void D3D11InputDevice::SetMousePosition(Math::FTVector2 pos)
	{
		mMousePosX = static_cast<unsigned int>(pos.x);
		mMousePosY = static_cast<unsigned int>(pos.y);
	}

	void D3D11InputDevice::SetMousePosition(unsigned int posX, unsigned int posY)
	{
		mMousePosX = posX;
		mMousePosY = posY;
	}

	void D3D11InputDevice::SetMouseWheelDelta(float delta)
	{
		mMouseWheelDelta = delta;
	}

	void D3D11InputDevice::Update(Core::IWindow* window)
	{
		MSG					msg = {};
		D3D11::D3D11Window* win = reinterpret_cast<D3D11::D3D11Window*>(window);
		if (PeekMessage(&msg, win->GetHandle(), 0, 0, PM_REMOVE))
		{
			// EditorCamera2D::GetInstance()->ProcessInput(msg);
		}
		DetectMouseInput(msg);
		DetectKeyboardInput();
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	void D3D11InputDevice::Reset()
	{
		mMousePosX = 0;
		mMousePosY = 0;
		mMouseWheelDelta = 0.f;
		mIsDragging		 = false;
	}

	D3D11InputDevice::D3D11InputDevice()
		: mMousePosX(0)
		, mMousePosY(0)
		, mMouseWheelDelta(0.f)
		, mIsDragging(false)
		, mMouseButtons(DBG_NEW Common::FTDS::DynamicArray<ButtonInput>)
		, mKeyboardButtons(DBG_NEW Common::FTDS::DynamicArray<ButtonInput>)
	{
		mKeyboardCode = DBG_NEW int[(size_t)KEYBOARD::LAST_FLAG]{
			'A',
			'D',
			'W',
			'S',
			VK_SHIFT,
			VK_SPACE
		};

		mMouseCode = DBG_NEW int[(size_t)MOUSE::LAST_FLAG]{
			VK_LBUTTON,
			VK_RBUTTON,
			VK_MBUTTON
		};

		mMouseButtons->Reserve((size_t)MOUSE::LAST_FLAG);
		for (size_t i = 0; i < (size_t)MOUSE::LAST_FLAG; ++i)
		{
			mMouseButtons->PushBack(ButtonInput{ ButtonState::NONE, false });
		}

		mKeyboardButtons->Reserve((size_t)KEYBOARD::LAST_FLAG);
		for (size_t i = 0; i < (size_t)KEYBOARD::LAST_FLAG; ++i)
		{
			mKeyboardButtons->PushBack(ButtonInput{ ButtonState::NONE, false });
		}
	}

	D3D11InputDevice::~D3D11InputDevice()
	{
		delete[] mMouseCode;
		delete mMouseButtons;

		delete[] mKeyboardCode;
		delete mKeyboardButtons;
	}
} // namespace D3D11