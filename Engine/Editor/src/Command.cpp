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

	/// @brief Applies the command's change to the target object.
	void IntEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void IntEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void IntEditCommand::SetNextVal(int nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement integer values for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :IntEditCommand base or delegates to its constructor.
	IntEditCommand::IntEditCommand(int& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(0)
	{
	}

	/// @brief Applies the command's change to the target object.
	void FloatEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void FloatEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void FloatEditCommand::SetNextVal(float nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement float values for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :FloatEditCommand base or delegates to its constructor.
	FloatEditCommand::FloatEditCommand(float& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(0.f)
	{
	}

	/// @brief Applies the command's change to the target object.
	void Vector2EditCommand::Do()
	{
		mPrevValue = mValue;
		mValue	   = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void Vector2EditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void Vector2EditCommand::SetNextVal(FTVector2 nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement two-dimensional vectors for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :Vector2EditCommand base or delegates to its constructor.
	Vector2EditCommand::Vector2EditCommand(FTVector2& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector2::Zero)
	{
	}

	/// @brief Applies the command's change to the target object.
	void WStrEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void WStrEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void WStrEditCommand::SetNextVal(std::wstring nextVal)
	{
		mNextValue.assign(nextVal);
	}

	/// @brief Initializes the previous and replacement wide strings for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :WStrEditCommand base or delegates to its constructor.
	WStrEditCommand::WStrEditCommand(std::wstring& valRef)
		: mPrevValue(valRef)
		, mValue(valRef)
		, mNextValue()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	WStrEditCommand::~WStrEditCommand()
	{
	}

	/// @brief Applies the command's change to the target object.
	void Vector3EditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void Vector3EditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Returns the prev val used by this vector3 edit command.
	/// @return Current prev val.
	FTVector3 Vector3EditCommand::GetPrevVal()
	{
		return mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void Vector3EditCommand::SetNextVal(FTVector3 nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement three-dimensional vectors for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :Vector3EditCommand base or delegates to its constructor.
	Vector3EditCommand::Vector3EditCommand(FTVector3& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector3::Zero)
	{
	}

	/// @brief Applies the command's change to the target object.
	void StrEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void StrEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void StrEditCommand::SetNextVal(Common::FTDS::String nextVal)
	{
		mNextValue.Assign(nextVal);
	}

	/// @brief Initializes the previous and replacement text values for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :StrEditCommand base or delegates to its constructor.
	StrEditCommand::StrEditCommand(Common::FTDS::String& valRef)
		: mPrevValue(valRef)
		, mValue(valRef)
		, mNextValue()
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	StrEditCommand::~StrEditCommand()
	{
	}

	/// @brief Initializes the previous and replacement boolean values for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :BoolEditCommand base or delegates to its constructor.
	BoolEditCommand::BoolEditCommand(bool& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(true)
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	BoolEditCommand::~BoolEditCommand()
	{
	}

	/// @brief Applies the command's change to the target object.
	void BoolEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void BoolEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the prev val used by subsequent operations.
	/// @param prevVal Replacement prev val.
	void BoolEditCommand::SetPrevVal(bool prevVal)
	{
		mPrevValue = prevVal;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void BoolEditCommand::SetNextVal(bool nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Applies the command's change to the target object.
	void ActorGroupEditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void ActorGroupEditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void ActorGroupEditCommand::SetNextVal(Common::ActorGroup nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement actor group for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :ActorGroupEditCommand base or delegates to its constructor.
	ActorGroupEditCommand::ActorGroupEditCommand(Common::ActorGroup& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(Common::ActorGroup::NOT_ASSIGNED)
	{
	}

	/// @brief Applies the command's change to the target object.
	void Vector4EditCommand::Do()
	{
		mValue = mNextValue;
	}

	/// @brief Restores the target state saved before the command was applied.
	void Vector4EditCommand::Undo()
	{
		mValue = mPrevValue;
	}

	/// @brief Updates the next val used by subsequent operations.
	/// @param nextVal Replacement next val.
	void Vector4EditCommand::SetNextVal(FTVector4 nextVal)
	{
		mNextValue = nextVal;
	}

	/// @brief Initializes the previous and replacement four-dimensional vectors for undo/redo.
	/// @param valRef Value edited by the UI control.
	/// @note Initializes the :Vector4EditCommand base or delegates to its constructor.
	Vector4EditCommand::Vector4EditCommand(FTVector4& valRef)
		: mPrevValue(valRef), mValue(valRef), mNextValue(FTVector4(0.f, 0.f, 0.f, 0.f))
	{
	}
} // namespace Editor
