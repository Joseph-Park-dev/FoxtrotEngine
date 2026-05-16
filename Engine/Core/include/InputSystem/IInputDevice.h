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
#include "FTMath.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	class IWindow;
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
		virtual void Update(Core::IWindow* window) = 0;

		/// <summary>
		/// Resets per-frame input state. Called at end of frame.
		/// </summary>
		virtual void Reset() = 0;

	public:
		//////////////////////////////////////////
		////// Keyboard Input ////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Checks if a key was just pressed this frame.
		/// </summary>
		/// <param name="keyCode">Platform-agnostic key code.</param>
		virtual bool IsKeyTap(int keyCode) const = 0;

		/// <summary>
		/// Checks if a key is currently held down.
		/// </summary>
		/// <param name="keyCode">Platform-agnostic key code.</param>
		virtual bool IsKeyHold(int keyCode) const = 0;

		/// <summary>
		/// Checks if a key was just released this frame.
		/// </summary>
		/// <param name="keyCode">Platform-agnostic key code.</param>
		virtual bool IsKeyAway(int keyCode) const = 0;

		/// <summary>
		/// Gets the current state of a keyboard key.
		/// </summary>
		/// <param name="keyCode">Platform-agnostic key code.</param>
		virtual ButtonState GetKeyState(int keyCode) const = 0;

		//////////////////////////////////////////
		////// Mouse Input ///////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Checks if a mouse button was just pressed this frame.
		/// </summary>
		/// <param name="button">Mouse button index (0=left, 1=right, 2=middle).</param>
		virtual bool IsMouseTap(int button) const = 0;

		/// <summary>
		/// Checks if a mouse button is currently held down.
		/// </summary>
		/// <param name="button">Mouse button index.</param>
		virtual bool IsMouseHold(int button) const = 0;

		/// <summary>
		/// Checks if a mouse button was just released this frame.
		/// </summary>
		/// <param name="button">Mouse button index.</param>
		virtual bool IsMouseAway(int button) const = 0;

		/// <summary>
		/// Gets the current mouse position in screen coordinates.
		/// </summary>
		virtual Math::FTVector2 GetMousePosition() const = 0;

		/// <summary>
		/// Gets the mouse movement delta since last frame.
		/// </summary>
		virtual Math::FTVector2 GetMouseDelta() const = 0;

		/// <summary>
		/// Gets the mouse wheel scroll delta.
		/// </summary>
		virtual const float GetMouseWheelDelta() const = 0;

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
} // namespace Core