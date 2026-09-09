// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Commands that occurr when adding, removing, deleting EditorElements
/// in the EditorScene. These are used in CommandHistory
/// </summary>

#pragma once
#include "Command.h"
#include "EditorElement.h"

namespace Editor
{
	class ActorCommand :
		public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Returns the val used by this actor command.
		/// @return Borrowed access to the val.
		EditorElement* GetVal();
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void		   SetNextVal(EditorElement* nextVal);

	public:
		/// @brief Initializes the actor target used by undoable editor operations.
		/// @param elem Editor element targeted by the operation.
		ActorCommand(EditorElement* elem);

	private:
		EditorElement* mValue;
		EditorElement* mPrevValue;
		EditorElement* mNextValue;
	};

	class ActorAdditionCommand :
		public ActorCommand
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Initializes an undoable actor insertion.
		/// @param elem Editor element targeted by the operation.
		ActorAdditionCommand(EditorElement* elem);
	};
} // namespace Editor
