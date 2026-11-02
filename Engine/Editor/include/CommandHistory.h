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

#include "Utility/SingletonMacro.h"
#include "FTDS/Static/ArrayStack.h"
#include "Plugin/EditorExports.h"
#include "Plugin/ICommandHistory.h"

#define COMMAND_MAXCOUNT 30

namespace Editor
{
	class ActorCommand;
	class Command;
	class CommandHistory : public ICommandHistory
	{
		SINGLETON(CommandHistory)
	public:
		/// @brief Records the command sequence used to support subsequent undo and redo.
		/// Push the current command to the previous,
		/// Flush out the next commands.
		void ArrangeCommand();
		/// @brief Updates the current used by subsequent operations.
		/// @param cmd Replacement current.
		void SetCurrent(Command* cmd);

	public:
		// These member functions will be used on Foxtrot Editor when updating values.
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateIntValue(const char* label, int& ref, int modSpeed = INTMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param min Inclusive lower bound.
		/// @param max Inclusive upper bound.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateIntValue(const char* label, int& ref, int min, int max, int modSpeed = INTMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateUnsignedIntValue(const char* label, unsigned int& ref, int modSpeed = INTMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateFloatValue(const char* label, float& ref, float modSpeed = FLOATMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		virtual void UpdateBoolValue(const char* label, bool& ref) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateVector2Value(const char* label, Math::FTVector2& ref, float modSpeed = FLOATMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateVector3Value(const char* label, Math::FTVector3& ref, float modSpeed = FLOATMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		/// @param modSpeed Playback or edit speed multiplier.
		virtual void UpdateVector4Value(const char* label, Math::FTVector4& ref, float modSpeed = FLOATMOD_SPEED) override;
		/// @brief Draws a typed editor control and applies changes to the referenced value.
		/// @param label Text identifying the editor control.
		/// @param ref Reference value used by the operation.
		virtual void UpdateStringValue(const char* label, Common::FTDS::String& ref) override;

	public:
		/// @brief Advances frame-dependent state using the current time step.
		void Update();
		// This will be called when closing a Foxtrot Editor instance.
		/// @brief Releases runtime services and resources during engine shutdown.
		void ShutDown();

	private:
		Common::FTDS::ArrayStack<Command*>* mPrevious;
		Common::FTDS::ArrayStack<Command*>* mNext;
		Command*							mCurrent;
		bool								mIsRecording; // A value is being modified on UI.

	private:
		// Get the command located at the pointer position.
		/// @brief Returns the current command used by this command history.
		/// @return Borrowed access to the current command.
		Command* GetCurrentCommand();

		// Navigates through the Commands.
		// This feature is not working properly in the current version.
		/// @brief Reverses the most recently applied command in the history.
		void UndoCommand();
		// This feature is not working properly in the current version.
		/// @brief Reapplies the next command in the redo history.
		void RedoCommand();
	};

	extern "C"
	{
		/// @brief Returns the command history used by editor value controls.
		/// @return Borrowed command history. The Editor owns the instance.
		EDITOR_API ICommandHistory* GetCMDHistory();
	}
} // namespace Editor
