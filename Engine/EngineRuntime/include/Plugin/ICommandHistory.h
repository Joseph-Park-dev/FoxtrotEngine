// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once

#ifdef FOXTROT_EDITOR
#include "FTDS/Static/FTString.h"

namespace Math
{
	class FTVector2;
	class FTVector3;
	class FTVector4;
} // namespace Math

namespace Editor
{
	constexpr int	INTMOD_SPEED   = 1;
	constexpr float	FLOATMOD_SPEED = 0.1f;

	/// <summary>
	/// Editor value controls recorded by the command history.
	/// Non-Editor modules obtain the instance from GetCMDHistory and call these
	/// methods without including CommandHistory.h.
	/// </summary>
	class ICommandHistory
	{
	public:
		static constexpr const char* INTERFACE_NAME = "CommandHistory.v1";

		/// @brief Draws an integer editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateIntValue(const char* label, int& ref, int modSpeed = INTMOD_SPEED) = 0;

		/// @brief Draws an integer editor control clamped to an inclusive range and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param min Inclusive lower bound.
		/// @param max Inclusive upper bound.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateIntValue(const char* label, int& ref, int min, int max, int modSpeed = INTMOD_SPEED) = 0;

		/// @brief Draws an unsigned integer editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateUnsignedIntValue(const char* label, unsigned int& ref, int modSpeed = INTMOD_SPEED) = 0;

		/// @brief Draws a float editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateFloatValue(const char* label, float& ref, float modSpeed = FLOATMOD_SPEED) = 0;

		/// @brief Draws a checkbox and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		virtual void UpdateBoolValue(const char* label, bool& ref) = 0;

		/// @brief Draws a two-component vector editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateVector2Value(const char* label, Math::FTVector2& ref, float modSpeed = FLOATMOD_SPEED) = 0;

		/// @brief Draws a three-component vector editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateVector3Value(const char* label, Math::FTVector3& ref, float modSpeed = FLOATMOD_SPEED) = 0;

		/// @brief Draws a four-component vector editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		/// @param modSpeed Drag speed passed to the editor control.
		virtual void UpdateVector4Value(const char* label, Math::FTVector4& ref, float modSpeed = FLOATMOD_SPEED) = 0;

		/// @brief Draws a text editor control and records the edit.
		/// @param label Text identifying the editor control.
		/// @param ref Value edited by the control.
		virtual void UpdateStringValue(const char* label, Common::FTDS::String& ref) = 0;

	protected:
		/// @brief Borrowed interface. Callers must not delete the Editor instance.
		virtual ~ICommandHistory() = default;
	};

	/// @brief Signature of the GetCMDHistory export.
	using GET_CMD_HISTORY = ICommandHistory* (*)();
} // namespace Editor
#endif // FOXTROT_EDITOR
