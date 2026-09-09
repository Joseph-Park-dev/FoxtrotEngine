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

		/// @brief Advances frame-dependent state using the current time step.
		/// Updates input state. Called once per frame before input processing.
		/// @param window Window used by the operation.
		virtual void Update(Graphics::IWindow* window) = 0;

		/// @brief Clears transient state before the next processing cycle.
		/// Resets per-frame input state. Called at end of frame.
		virtual void Reset() = 0;

	public:
		/// @brief Returns the mouse pos x used by this iinput device.
		/// ///////////////////////////////////////
		/// /// Mouse Input ///////////////////////
		/// ///////////////////////////////////////
		/// Gets the current mouse position in screen coordinates.
		/// @return Current mouse pos x.
		virtual int GetMousePosX() const = 0;
		/// @brief Returns the mouse pos y used by this iinput device.
		/// @return Current mouse pos y.
		virtual int GetMousePosY() const = 0;

		/// @brief Returns the mouse wheel delta used by this iinput device.
		/// Gets the mouse wheel scroll delta.
		/// @return Current mouse wheel delta.
		virtual const float GetMouseWheelDelta() const = 0;

	public:
		/// @brief Tests whether the input remains pressed across frames.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input remains pressed across frames; otherwise false.
		virtual bool KEY_HOLD(KEYBOARD key) = 0;
		/// @brief Tests whether the input transitioned to pressed this frame.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input transitioned to pressed this frame; otherwise false.
		virtual bool KEY_TAP(KEYBOARD key)	= 0;
		/// @brief Tests whether the input transitioned to released this frame.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input transitioned to released this frame; otherwise false.
		virtual bool KEY_AWAY(KEYBOARD key) = 0;
		/// @brief Tests whether the input is neither pressed nor transitioning.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input is neither pressed nor transitioning; otherwise false.
		virtual bool KEY_NONE(KEYBOARD key) = 0;

		/// @brief Tests whether the input remains pressed across frames.
		/// @param mouse Mouse button to query.
		/// @return True when the input remains pressed across frames; otherwise false.
		virtual bool MOUSE_HOLD(MOUSE mouse) = 0;
		/// @brief Tests whether the input transitioned to pressed this frame.
		/// @param mouse Mouse button to query.
		/// @return True when the input transitioned to pressed this frame; otherwise false.
		virtual bool MOUSE_TAP(MOUSE mouse)	 = 0;
		/// @brief Tests whether the input transitioned to released this frame.
		/// @param mouse Mouse button to query.
		/// @return True when the input transitioned to released this frame; otherwise false.
		virtual bool MOUSE_AWAY(MOUSE mouse) = 0;
		/// @brief Tests whether the input is neither pressed nor transitioning.
		/// @param mouse Mouse button to query.
		/// @return True when the input is neither pressed nor transitioning; otherwise false.
		virtual bool MOUSE_NONE(MOUSE mouse) = 0;

		/// @brief Reads the stored cursor X coordinate in client pixels.
		/// @return Signed cursor coordinate in client pixels.
		virtual int MOUSE_X() = 0;
		/// @brief Reads the stored cursor Y coordinate in client pixels.
		/// @return Signed cursor coordinate in client pixels.
		virtual int MOUSE_Y() = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IInputDevice() = default;

	protected:
		/// @brief Returns the button input used by this iinput device.
		/// @param btnArr btn arr used by this operation.
		/// @param button Mouse button or UI button identifier.
		/// @return Borrowed access to the button input.
		/// @note Changes through the returned reference affect this object's stored state.
		template <typename BUTTON_TYPE>
		ButtonInput& GetButtonInput(Common::FTDS::DynamicArray<ButtonInput>* btnArr, BUTTON_TYPE button)
		{
			return btnArr->At((size_t)button);
		}

		/// @brief Returns the button input used by this iinput device.
		/// @param btnArr btn arr used by this operation.
		/// @param buttonIdx Index of the requested button.
		/// @return Borrowed access to the button input.
		/// @note Changes through the returned reference affect this object's stored state.
		ButtonInput& GetButtonInput(Common::FTDS::DynamicArray<ButtonInput>* btnArr, size_t buttonIdx)
		{
			return btnArr->At(buttonIdx);
		}

		/// @brief Returns the button state used by this iinput device.
		/// @param btnArr btn arr used by this operation.
		/// @param button Mouse button or UI button identifier.
		/// @return Borrowed access to the button state.
		/// @note Changes through the returned reference affect this object's stored state.
		template <typename BUTTON_TYPE>
		ButtonState& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, BUTTON_TYPE button)
		{
			return btnArr->At((size_t)button).ButtonState;
		}

		/// @brief Returns the button state used by this iinput device.
		/// @param btnArr btn arr used by this operation.
		/// @param buttonIdx Index of the requested button.
		/// @return Borrowed access to the button state.
		/// @note Changes through the returned reference affect this object's stored state.
		ButtonState& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, size_t buttonIdx)
		{
			return btnArr->At(buttonIdx).ButtonState;
		}
	};
} // namespace InputSystem
