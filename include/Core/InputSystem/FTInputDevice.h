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
#include "Math/FTMath.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	enum class BUTTON_STATE
	{
		NONE,
		TAP,
		HOLD,
		AWAY
	};

	struct ButtonInput
	{
		BUTTON_STATE ButtonState;
		bool		 IsPushedPrevFrame;
	};

#define MAX_KEYCOUNT 6

	class FTInputDevice
	{
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
		BUTTON_STATE& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, BUTTON_TYPE button)
		{
			return btnArr->At((size_t)button).ButtonState;
		}

		BUTTON_STATE& GetButtonState(Common::FTDS::DynamicArray<ButtonInput>* btnArr, size_t buttonIdx)
		{
			return btnArr->At(buttonIdx).ButtonState;
		}
	};
} // namespace Core