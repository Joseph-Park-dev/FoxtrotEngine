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
#include <deque>
#include <functional>

#include "Core/SingletonMacro.h"
#include "Actors/Transform.h"
#include "Command.h"

#define COMMAND_MAXCOUNT 30
#define FLOATMOD_SPEED 0.1f
#define INTMOD_SPEED 1

class ActorCommand;

class CommandHistory
{
	SINGLETON(CommandHistory)

public:
	// Maually adds command to deque.
	void AddCommand(Command* command);

	// These member functions will be used on Foxtrot Editor when updating values.
	void UpdateIntValue(std::string label, int& ref, int modSpeed = INTMOD_SPEED);
	void UpdateFloatValue(std::string label, float& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateBoolValue(std::string label, bool& ref);
	void UpdateVector2Value(std::string label, FTVector2& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector2Value(std::string label, b2Vec2& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector3Value(std::string label, FTVector3& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector3Value(std::string label, DirectX::SimpleMath::Vector3& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateStringValue(std::string label, std::string& ref);

	void UpdateStateValue(std::string label, Actor::State& state);

public:
	void UpdateActorAddition(EditorElement* editorElement);

public:
	ActorCommand* GetLatestActorCommand();
	void		  SetLatestActorCommand(ActorCommand* command);

public:
	void Update();
	// This will be called when closing a Foxtrot Editor instance.
	void ShutDown();

private:
	std::deque<Command*> mCommandDeq;
	size_t				 mCommandPointer;  // This points to the position of the latest Command created.
	size_t				 mCMDStartPointer; // This points to the value from the frame that a value started to be modified.
	size_t				 mCMDEndPointer;   // When the recording is finished, the intermediate commands from start to this point is deleted.
	bool				 mIsRecording;	   // A value is being modified on UI.

	// mCommandPointer position of the latest EditorElement
	// This is used in Actor related Commands
	ActorCommand* mLatestActorCommand;

private:
	// When modifying values on UI, CMDRecord is started
	// e.g. dragging a slider.
	void StartCMDRecord();

	// CMDRecord is ended when the modification is finished
	// e.g. releasing a slider.
	void EndCMDRecord();

	// When command pointer is not at the top of the deque
	void MergeCMDRecord();

	// Get the command located at the pointer position.
	Command* GetCurrentCommand();

	// Navigates through the Commands.
	// This feature is not working properly in the current version.
	void UndoCommand();
	// This feature is not working properly in the current version.
	void RedoCommand();
};