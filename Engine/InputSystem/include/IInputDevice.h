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
#include "FTDS/Dynamic/DynamicArray.h"

namespace Graphics
{
	class IWindow;
}

namespace InputSystem
{
	enum class KEYBOARD
	{
		A,
		D,
		W,
		S,
		SHIFT,
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

	enum class ButtonState
	{
		NONE,
		TAP,
		HOLD,
		AWAY
	};

	struct ButtonInput
	{
		ButtonState ButtonState;
		bool		IsPushedPrevFrame;
	};

	class IWindow;
#define MAX_KEYCOUNT 6

	class IInputDevice
	{
	public:
		//////////////////////////////////////////
		////// Frame Update //////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Updates input state. Called once per frame before input processing.
		/// </summary>
		virtual void Update(Graphics::IWindow* window) = 0;

		/// <summary>
		/// Resets per-frame input state. Called at end of frame.
		/// </summary>
		virtual void Reset() = 0;

	public:
		//////////////////////////////////////////
		////// Mouse Input ///////////////////////
		//////////////////////////////////////////
		/// <summary>
		/// Gets the current mouse position in screen coordinates.
		/// </summary>
		virtual const unsigned int GetMousePosX() const = 0;
		virtual const unsigned int GetMousePosY() const = 0;

		/// <summary>
		/// Gets the mouse wheel scroll delta.
		/// </summary>
		virtual const float GetMouseWheelDelta() const = 0;

	public:
		virtual bool KEY_HOLD(KEYBOARD key) = 0;
		virtual bool KEY_TAP(KEYBOARD key)	= 0;
		virtual bool KEY_AWAY(KEYBOARD key) = 0;
		virtual bool KEY_NONE(KEYBOARD key) = 0;

		virtual bool MOUSE_HOLD(MOUSE mouse) = 0;
		virtual bool MOUSE_TAP(MOUSE mouse)	 = 0;
		virtual bool MOUSE_AWAY(MOUSE mouse) = 0;
		virtual bool MOUSE_NONE(MOUSE mouse) = 0;

		virtual unsigned int MOUSE_X() = 0;
		virtual unsigned int MOUSE_Y() = 0;

	public:
		virtual ~IInputDevice() = default;

	protected:
		template <typename BUTTON_TYPE>
		ButtonInput& GetButtonInput(Common::FTDS::DynamicArray<ButtonInput>* btnArr, BUTTON_TYPE button)
		{
			return btnArr->At((size_t)button);
		}

		ButtonInput& GetButtonInput(Common::FTDS::DynamicArray<ButtonInput>* btnArr, size_t buttonIdx)
		{
			return btnArr->At(buttonIdx);
		}

		template <typename BUTTON_TYPE>
		ButtonState& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, BUTTON_TYPE button)
		{
			return btnArr->At((size_t)button).ButtonState;
		}

		ButtonState& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, size_t buttonIdx)
		{
			return btnArr->At(buttonIdx).ButtonState;
		}
	};
} // namespace InputSystem