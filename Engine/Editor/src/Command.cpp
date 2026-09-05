// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Command.h"

#include "FTMath.h"
#include "TemplateFunctions.h"
#include "Actor/Actor.h"
#include "Actor/ActorGroup.h"

namespace Editor
{
	using namespace Math;
	using namespace Core;

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

	void StrEditCommand::Do()
	{
		mValue = mNextValue;
	}

	void StrEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	void StrEditCommand::SetNextVal(Common::FTDS::String nextVal)
	{
		mNextValue.Assign(nextVal);
	}

	StrEditCommand::StrEditCommand(Common::FTDS::String& valRef)
		: mPrevValue(valRef)
		, mValue(valRef)
		, mNextValue()
	{
	}

	StrEditCommand::~StrEditCommand()
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

	void BoolEditCommand::SetPrevVal(bool prevVal)
	{
		mPrevValue = prevVal;
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

	void ActorGroupEditCommand::SetNextVal(Common::ActorGroup nextVal)
	{
		mNextValue = nextVal;
	}

	ActorGroupEditCommand::ActorGroupEditCommand(Common::ActorGroup& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(Common::ActorGroup::NOT_ASSIGNED)
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
} // namespace Editor