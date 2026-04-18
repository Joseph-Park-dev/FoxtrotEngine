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
		void Do() override;
		void Undo() override;

	public:
		EditorElement* GetVal();
		void		   SetNextVal(EditorElement* nextVal);

	public:
		ActorCommand(EditorElement* elem);

	private:
		EditorElement* mValue;
		EditorElement  mPrevValue;
		EditorElement  mNextValue;
	};

	class ActorAdditionCommand :
		public ActorCommand
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		ActorAdditionCommand(EditorElement* elem);
	};
} // namespace Editor