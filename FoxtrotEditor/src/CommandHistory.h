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
#include <functional>
#include <directxtk/SimpleMath.h>

#include "Core/SingletonMacro.h"
#include "Actors/Transform.h"
#include "Command.h"
#include "Static/ArrayStack.h"

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
	void UpdateIntValue(const char* label, int& ref, int modSpeed = INTMOD_SPEED);
	void UpdateIntValue(const char* label, int& ref, int min, int max, int modSpeed = INTMOD_SPEED);
	void UpdateUnsignedIntValue(const char* label, UINT& ref, UINT modSpeed = INTMOD_SPEED);
	void UpdateFloatValue(const char* label, float& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateBoolValue(const char* label, bool& ref);
	void UpdateVector2Value(const char* label, FTVector2& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector2Value(const char* label, DirectX::SimpleMath::Vector2& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector2Value(const char* label, b2Vec2& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector3Value(const char* label, FTVector3& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector3Value(const char* label, DirectX::SimpleMath::Vector3& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateVector4Value(const char* label, FTVector4& ref, float modSpeed = FLOATMOD_SPEED);
	void UpdateStringValue(const char* label, FTDS::String& ref);

	void UpdateStateValue(const char* label, Actor::State& state);

public:
	void Update();
	// This will be called when closing a Foxtrot Editor instance.
	void ShutDown();

private:
	FTDS::ArrayStack<Command*>* mPrevious;
	FTDS::ArrayStack<Command*>* mNext;
	Command*					mCurrent;
	bool						mIsRecording; // A value is being modified on UI.

private:
	// Get the command located at the pointer position.
	Command* GetCurrentCommand();

	// Navigates through the Commands.
	// This feature is not working properly in the current version.
	void UndoCommand();
	// This feature is not working properly in the current version.
	void RedoCommand();
};