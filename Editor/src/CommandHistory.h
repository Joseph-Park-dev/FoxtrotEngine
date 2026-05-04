// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton object that takes care of a deque of commands.
/// This records a value as commands when updating it.
///
/// NOTE : The Undo-Redo is not working properly in the current version.
/// </summary>

#pragma once
#include "Entity/Entity.h"

#include "SingletonMacro.h"
#include "FTDS/Static/ArrayStack.h"
#include "Plugin/EditorExports.h"
#include "FTDS/Static/FTString.h"

#define COMMAND_MAXCOUNT 30
#define FLOATMOD_SPEED 0.1f
#define INTMOD_SPEED 1

namespace Math
{
	class FTVector2;
	class FTVector3;
	class FTVector4;
} // namespace Math

namespace Editor
{
	using UPDATE_INT_VAL		  = void (*)(const char* label, int& ref);
	using UPDATE_CLAMPED_INT_VAL  = void (*)(const char* label, int& ref, int min, int max);
	using UPDATE_UNSIGNED_INT_VAL = void (*)(const char* label, unsigned int& ref);
	using UPDATE_FLOAT_VAL		  = void (*)(const char* label, float& ref);
	using UPDATE_BOOL_VAL		  = void (*)(const char* label, bool& ref);
	using UPDATE_VEC2_VAL		  = void (*)(const char* label, Math::FTVector2& ref);
	using UPDATE_VEC3_VAL		  = void (*)(const char* label, Math::FTVector3& ref);
	using UPDATE_VEC4_VAL		  = void (*)(const char* label, Math::FTVector4& ref);
	using UPDATE_STR_VAL		  = void (*)(const char* label, Common::FTDS::String& ref);

	class ActorCommand;
	class Command;
	class CommandHistory : public Core::Entity
	{
		SINGLETON(CommandHistory)
	public:
		/// <summary>
		/// Push the current command to the previous,
		/// Flush out the next commands.
		/// </summary>
		void ArrangeCommand();
		void SetCurrent(Command* cmd);

	public:
		// These member functions will be used on Foxtrot Editor when updating values.
		void UpdateIntValue(const char* label, int& ref, unsigned int modSpeed = INTMOD_SPEED);
		void UpdateIntValue(const char* label, int& ref, int min, int max, unsigned int modSpeed = INTMOD_SPEED);
		void UpdateUnsignedIntValue(const char* label, unsigned int& ref, unsigned int modSpeed = INTMOD_SPEED);
		void UpdateFloatValue(const char* label, float& ref, float modSpeed = FLOATMOD_SPEED);
		void UpdateBoolValue(const char* label, bool& ref);
		void UpdateVector2Value(const char* label, Math::FTVector2& ref, float modSpeed = FLOATMOD_SPEED);
		void UpdateVector3Value(const char* label, Math::FTVector3& ref, float modSpeed = FLOATMOD_SPEED);
		void UpdateVector4Value(const char* label, Math::FTVector4& ref, float modSpeed = FLOATMOD_SPEED);
		void UpdateStringValue(const char* label, Common::FTDS::String& ref);

	public:
		void Update();
		// This will be called when closing a Foxtrot Editor instance.
		void ShutDown();

	private:
		Common::FTDS::ArrayStack<Command*>* mPrevious;
		Common::FTDS::ArrayStack<Command*>* mNext;
		Command*							mCurrent;
		bool								mIsRecording; // A value is being modified on UI.

	private:
		// Get the command located at the pointer position.
		Command* GetCurrentCommand();

		// Navigates through the Commands.
		// This feature is not working properly in the current version.
		void UndoCommand();
		// This feature is not working properly in the current version.
		void RedoCommand();
	};

	extern "C"
	{
		EDITOR_API CommandHistory* GetCMDHistory();

		EDITOR_API void UpdateIntValue(const char* label, int& ref, unsigned int modSpeed = INTMOD_SPEED);
		EDITOR_API void UpdateClampedIntValue(const char* label, int& ref, int min, int max, int modSpeed = INTMOD_SPEED);
		EDITOR_API void UpdateUnsignedIntValue(const char* label, unsigned int& ref, unsigned int modSpeed = INTMOD_SPEED);
		EDITOR_API void UpdateFloatValue(const char* label, float& ref, float modSpeed = FLOATMOD_SPEED);
		EDITOR_API void UpdateBoolValue(const char* label, bool& ref);
		EDITOR_API void UpdateVector2Value(const char* label, Math::FTVector2& ref, float modSpeed = FLOATMOD_SPEED);
		EDITOR_API void UpdateVector3Value(const char* label, Math::FTVector3& ref, float modSpeed = FLOATMOD_SPEED);
		EDITOR_API void UpdateVector4Value(const char* label, Math::FTVector4& ref, float modSpeed = FLOATMOD_SPEED);
		EDITOR_API void UpdateStringValue(const char* label, Common::FTDS::String& ref);
	}
	using GET_CMD_HISTORY = Editor::CommandHistory* (*)();
} // namespace Editor