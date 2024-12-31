// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Command.h"

#include <string>

#include "Math/FTMath.h"
#include "Core/TemplateFunctions.h"
#include "Actors/Actor.h"

Command::Command()
{
}

void IntEditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void IntEditCommand::Undo()
{
	mValue = mPrevValue;
}

IntEditCommand::IntEditCommand(int& valRef, int nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void IntEditCommandPtr::Execute()
{
	mPrevValue = *mValue;
	*mValue = mNextValue;
}

void IntEditCommandPtr::Undo()
{
	int nextVal = *mValue;
	*mValue = mPrevValue;
	mNextValue = nextVal;
}

IntEditCommandPtr::IntEditCommandPtr(int* valRef, int nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void FloatEditCommand::Execute()
{
	mPrevValue = *mValue;
	*mValue = mNextValue;
}

void FloatEditCommand::Undo()
{
	float nextVal = *mValue;
	*mValue = mPrevValue;
	mNextValue = nextVal;
}

FloatEditCommand::FloatEditCommand(float* valRef, float nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void Vector2EditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void Vector2EditCommand::Undo()
{
	FTVector2 nextVal = mValue;
	mValue = mPrevValue;
	mNextValue = nextVal;
}

Vector2EditCommand::Vector2EditCommand(FTVector2& valRef, FTVector2 nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void WStrEditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void WStrEditCommand::Undo()
{
	mValue = mPrevValue;
}

WStrEditCommand::WStrEditCommand(std::wstring& valRef, std::wstring nextVal)
	: mPrevValue()
	, mValue	(valRef)
	, mNextValue(nextVal)
{}

WStrEditCommand::~WStrEditCommand()
{}

void B2Vec2EditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void B2Vec2EditCommand::Undo()
{
	mValue = mPrevValue;
}

B2Vec2EditCommand::B2Vec2EditCommand(b2Vec2& valRef, b2Vec2 nextVal)
	: mPrevValue()
	, mValue(valRef)
	, mNextValue(nextVal)
{}

void Vector3EditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void Vector3EditCommand::Undo()
{
	FTVector3 nextVal = mValue;
	mValue = mPrevValue;
	mNextValue = nextVal;
}

Vector3EditCommand::Vector3EditCommand(FTVector3& valRef, FTVector3 nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void DXVector3EditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void DXVector3EditCommand::Undo()
{
	DirectX::SimpleMath::Vector3 nextVal = mValue;
	mValue = mPrevValue;
	mNextValue = nextVal;
}

DXVector3EditCommand::DXVector3EditCommand(DirectX::SimpleMath::Vector3& valRef, DirectX::SimpleMath::Vector3 nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void StrEditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void StrEditCommand::Undo()
{
	mValue = mPrevValue;
}

StrEditCommand::StrEditCommand(std::string& valRef, std::string nextVal)
	: mPrevValue()
	, mValue(valRef)
	, mNextValue(nextVal)
{}

StrEditCommand::~StrEditCommand()
{
}

void StateEditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void StateEditCommand::Undo()
{
	mValue = mPrevValue;
}

StateEditCommand::StateEditCommand(Actor::State& valRef, Actor::State nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

BoolEditCommand::BoolEditCommand(bool& valRef, bool nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}


BoolEditCommand::~BoolEditCommand()
{}

void BoolEditCommand::Execute()
{
	mPrevValue = mValue;
	mValue = mNextValue;
}

void BoolEditCommand::Undo()
{
	mValue = mPrevValue;
}
