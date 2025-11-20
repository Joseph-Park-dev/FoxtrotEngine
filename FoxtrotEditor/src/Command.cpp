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

void IntEditCommand::Do()
{
	mValue = mNextValue;
}

void IntEditCommand::Undo()
{
	mValue = mPrevValue;
}

void IntEditCommand::SetNextVal(int nextVal)
{
	mNextValue = nextVal;
}

IntEditCommand::IntEditCommand(int& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(0)
{
}

void FloatEditCommand::Do()
{
	mValue = mNextValue;
}

void FloatEditCommand::Undo()
{
	mValue = mPrevValue;
}

void FloatEditCommand::SetNextVal(float nextVal)
{
	mNextValue = nextVal;
}

FloatEditCommand::FloatEditCommand(float& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(0.f)
{
}

void Vector2EditCommand::Do()
{
	mPrevValue = mValue;
	mValue	   = mNextValue;
}

void Vector2EditCommand::Undo()
{
	mValue = mPrevValue;
}

void Vector2EditCommand::SetNextVal(FTVector2 nextVal)
{
	mNextValue = nextVal;
}

Vector2EditCommand::Vector2EditCommand(FTVector2& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector2::Zero)
{
}

void WStrEditCommand::Do()
{
	mValue = mNextValue;
}

void WStrEditCommand::Undo()
{
	mValue = mPrevValue;
}

void WStrEditCommand::SetNextVal(std::wstring nextVal)
{
	mNextValue.assign(nextVal);
}

WStrEditCommand::WStrEditCommand(std::wstring& valRef)
	: mPrevValue(valRef)
	, mValue(valRef)
	, mNextValue()
{
}

WStrEditCommand::~WStrEditCommand()
{
}

void B2Vec2EditCommand::Do()
{
	mValue = mNextValue;
}

void B2Vec2EditCommand::Undo()
{
	mValue = mPrevValue;
}

void B2Vec2EditCommand::SetNextVal(b2Vec2 nextVal)
{
	mNextValue = nextVal;
}

B2Vec2EditCommand::B2Vec2EditCommand(b2Vec2& valRef)
	: mPrevValue(valRef)
	, mValue(valRef)
	, mNextValue(b2Vec2_zero)
{
}

void Vector3EditCommand::Do()
{
	mValue = mNextValue;
}

void Vector3EditCommand::Undo()
{
	mValue = mPrevValue;
}

FTVector3 Vector3EditCommand::GetPrevVal()
{
	return mPrevValue;
}

void Vector3EditCommand::SetNextVal(FTVector3 nextVal)
{
	mNextValue = nextVal;
}

Vector3EditCommand::Vector3EditCommand(FTVector3& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector3::Zero)
{
}

void DXVector3EditCommand::Do()
{
	mValue = mNextValue;
}

void DXVector3EditCommand::Undo()
{
	mValue = mPrevValue;
}

void DXVector3EditCommand::SetNextVal(DirectX::SimpleMath::Vector3 nextVal)
{
	mNextValue = nextVal;
}

DXVector3EditCommand::DXVector3EditCommand(DirectX::SimpleMath::Vector3& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(DirectX::SimpleMath::Vector3(0.0f))
{
}

void StrEditCommand::Do()
{
	mValue = mNextValue;
}

void StrEditCommand::Undo()
{
	mValue = mPrevValue;
}

void StrEditCommand::SetNextVal(FTDS::String nextVal)
{
	mNextValue.Assign(nextVal);
}

StrEditCommand::StrEditCommand(FTDS::String& valRef)
	: mPrevValue(valRef)
	, mValue(valRef)
	, mNextValue()
{
}

StrEditCommand::~StrEditCommand()
{
}

void ActorStateEditCommand::Do()
{
	mValue = mNextValue;
}

void ActorStateEditCommand::Undo()
{
	mValue = mPrevValue;
}

void ActorStateEditCommand::SetNextVal(Actor::State nextVal)
{
	mNextValue = nextVal;
}

ActorStateEditCommand::ActorStateEditCommand(Actor::State& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(Actor::State::EActive)
{
}

BoolEditCommand::BoolEditCommand(bool& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(true)
{
}

BoolEditCommand::~BoolEditCommand()
{
}

void BoolEditCommand::Do()
{
	mValue = mNextValue;
}

void BoolEditCommand::Undo()
{
	mValue = mPrevValue;
}

void BoolEditCommand::SetNextVal(bool nextVal)
{
	mNextValue = nextVal;
}

void ActorGroupEditCommand::Do()
{
	mValue = mNextValue;
}

void ActorGroupEditCommand::Undo()
{
	mValue = mPrevValue;
}

void ActorGroupEditCommand::SetNextVal(ActorGroup nextVal)
{
	mNextValue = nextVal;
}

ActorGroupEditCommand::ActorGroupEditCommand(ActorGroup& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(ActorGroup::NOT_ASSIGNED)
{
}

void Vector4EditCommand::Do()
{
	mValue = mNextValue;
}

void Vector4EditCommand::Undo()
{
	mValue = mPrevValue;
}

void Vector4EditCommand::SetNextVal(FTVector4 nextVal)
{
	mNextValue = nextVal;
}

Vector4EditCommand::Vector4EditCommand(FTVector4& valRef)
	: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector4(0.f, 0.f, 0.f, 0.f))
{
}
