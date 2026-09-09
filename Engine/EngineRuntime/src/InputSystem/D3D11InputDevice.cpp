#include "InputSystem/D3D11InputDevice.h"

#include <Windows.h>
#include <windowsx.h>

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTMath.h"
#include "Debugging/DebugMemAlloc.h"
#include "Renderer/IWindow.h"
#include "Foxtrot/Runtime/PlatformApi.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	/// @brief Samples keyboard state and records press, hold, and release transitions.
	void D3D11InputDevice::DetectKeyboardInput()
	{
		for (size_t i = 0; i < (size_t)KEYBOARD::LAST_FLAG; ++i)
		{
			ButtonInput& btnInput = GetButtonInput(mKeyboardButtons, i);
			if (GetAsyncKeyState(mKeyboardCode[i]) & 0x8000)
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

	/// @brief Updates mouse button and client-coordinate state from a Windows message.
	/// @param msg Windows message containing input or window data.
	void D3D11InputDevice::DetectMouseInput(MSG msg)
	{
        // WM_MOUSEMOVE is client-relative and signed; (0,0) is a valid update.
        if (msg.message == WM_MOUSEMOVE)
            SetMousePosition(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam));

		if (msg.message == WM_MOUSEWHEEL)
			SetMouseWheelDelta(GET_WHEEL_DELTA_WPARAM(msg.wParam));
		else
			SetMouseWheelDelta(0.f);

		for (size_t i = 0; i < (size_t)MOUSE::LAST_FLAG; ++i)
		{
			ButtonInput& mouse = GetButtonInput(mMouseButtons, i);
			if (GetAsyncKeyState(mMouseCode[i]) & 0x8000)
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

	/// @brief Computes mouse movement deltas for drag handling.
	/// @param deltaX Receives horizontal mouse movement.
	/// @param deltaY Receives vertical mouse movement.
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

	/// @brief Tests whether the input remains pressed across frames.
	/// @param key Lookup key identifying the stored entry.
	/// @return True when the input remains pressed across frames; otherwise false.
	bool D3D11InputDevice::KEY_HOLD(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::HOLD; }
	/// @brief Tests whether the input transitioned to pressed this frame.
	/// @param key Lookup key identifying the stored entry.
	/// @return True when the input transitioned to pressed this frame; otherwise false.
	bool D3D11InputDevice::KEY_TAP(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::TAP; }
	/// @brief Tests whether the input transitioned to released this frame.
	/// @param key Lookup key identifying the stored entry.
	/// @return True when the input transitioned to released this frame; otherwise false.
	bool D3D11InputDevice::KEY_AWAY(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::AWAY; }
	/// @brief Tests whether the input is neither pressed nor transitioning.
	/// @param key Lookup key identifying the stored entry.
	/// @return True when the input is neither pressed nor transitioning; otherwise false.
	bool D3D11InputDevice::KEY_NONE(KEYBOARD key) { return GetButtonState(mKeyboardButtons, key) == ButtonState::NONE; }

	/// @brief Tests whether the input remains pressed across frames.
	/// @param mouse Mouse button to query.
	/// @return True when the input remains pressed across frames; otherwise false.
	bool D3D11InputDevice::MOUSE_HOLD(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::HOLD; }
	/// @brief Tests whether the input transitioned to pressed this frame.
	/// @param mouse Mouse button to query.
	/// @return True when the input transitioned to pressed this frame; otherwise false.
	bool D3D11InputDevice::MOUSE_TAP(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::TAP; }
	/// @brief Tests whether the input transitioned to released this frame.
	/// @param mouse Mouse button to query.
	/// @return True when the input transitioned to released this frame; otherwise false.
	bool D3D11InputDevice::MOUSE_AWAY(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::AWAY; }
	/// @brief Tests whether the input is neither pressed nor transitioning.
	/// @param mouse Mouse button to query.
	/// @return True when the input is neither pressed nor transitioning; otherwise false.
	bool D3D11InputDevice::MOUSE_NONE(MOUSE mouse) { return GetButtonState(mMouseButtons, mouse) == ButtonState::NONE; }

	/// @brief Reads the stored cursor X coordinate in client pixels.
	/// @return Signed cursor coordinate in client pixels.
	int D3D11InputDevice::MOUSE_X() { return mMousePosX; }
	/// @brief Reads the stored cursor Y coordinate in client pixels.
	/// @return Signed cursor coordinate in client pixels.
	int D3D11InputDevice::MOUSE_Y() { return mMousePosY; }

	/// @brief Confines the cursor to the scene viewport while it is being manipulated.
	/// @param window Window used by the operation.
	/// @param mousePos Cursor position used for viewport interaction.
	void D3D11InputDevice::LockCursorInSceneViewport(Graphics::IWindow* window, Math::FTVector2 mousePos)
	{
		RECT rect;
		GetClientRect(static_cast<HWND>(window->NativeHandle()), &rect);

		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;

		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;

		MapWindowPoints(static_cast<HWND>(window->NativeHandle()), nullptr, &ul, 1);
		MapWindowPoints(static_cast<HWND>(window->NativeHandle()), nullptr, &lr, 1);

		rect.left = ul.x;
		rect.top  = ul.y;

		rect.right	= lr.x;
		rect.bottom = lr.y;
		ClipCursor(&rect);
	}

	/// @brief Releases the cursor confinement applied for viewport interaction.
	void D3D11InputDevice::UnlockCursorOutOfSceneViewport()
	{
		ClipCursor(nullptr);
	}

	/// @brief Returns the mouse pos x used by this d3 d11 input device.
	/// @return Current mouse pos x.
	int D3D11InputDevice::GetMousePosX() const
	{
		return mMousePosX;
	}

	/// @brief Returns the mouse pos y used by this d3 d11 input device.
	/// @return Current mouse pos y.
	int D3D11InputDevice::GetMousePosY() const
	{
		return mMousePosY;
	}

	/// @brief Returns the mouse wheel delta used by this d3 d11 input device.
	/// @return Current mouse wheel delta.
	const float D3D11InputDevice::GetMouseWheelDelta() const
	{
		return mMouseWheelDelta;
	}

	/// @brief Updates the mouse position used by subsequent operations.
	/// @param pos Replacement mouse position.
	void D3D11InputDevice::SetMousePosition(Math::FTVector2 pos)
	{
		mMousePosX = static_cast<int>(pos.x);
		mMousePosY = static_cast<int>(pos.y);
	}

	/// @brief Updates the mouse position used by subsequent operations.
	/// @param posX Horizontal position.
	/// @param posY Vertical position.
	void D3D11InputDevice::SetMousePosition(int posX, int posY)
	{
		mMousePosX = posX;
		mMousePosY = posY;
	}

	/// @brief Updates the mouse wheel delta used by subsequent operations.
	/// @param delta Replacement mouse wheel delta.
	void D3D11InputDevice::SetMouseWheelDelta(float delta)
	{
		mMouseWheelDelta = delta;
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param window Window used by the operation.
	void D3D11InputDevice::Update(Core::IWindow* window)
	{
        POINT pos{};
        if (GetCursorPos(&pos) && ScreenToClient(static_cast<HWND>(window->NativeHandle()), &pos))
            SetMousePosition(pos.x, pos.y);
        DetectMouseInput(MSG{});
        SetMouseWheelDelta(FtMouseWheelDelta());
        DetectKeyboardInput();
    }

	/// @brief Clears transient state before the next processing cycle.
	void D3D11InputDevice::Reset()
	{
		mMousePosX = 0;
		mMousePosY = 0;
		mMouseWheelDelta = 0.f;
		mIsDragging		 = false;
	}

	/// @brief Initializes keyboard and mouse state used for frame transitions.
	/// @note Initializes the :D3D11InputDevice base or delegates to its constructor.
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

	/// @brief Releases the resources managed by this instance during destruction.
	D3D11InputDevice::~D3D11InputDevice()
	{
		delete[] mMouseCode;
		delete mMouseButtons;

		delete[] mKeyboardCode;
		delete mKeyboardButtons;
	}
} // namespace D3D11
