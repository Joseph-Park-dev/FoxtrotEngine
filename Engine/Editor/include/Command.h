// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// An event-like object to be enqueued when changing values from Foxtrot Editor.
/// </summary>

#pragma once
#include "FTMath.h"
#include "FTDS/Static/FTString.h"
#include "Actor/ActorGroup.h"

namespace Math
{
	class FTVector2;
	class FTVector3;
	class FTVector4;
} // namespace Math

namespace Editor
{
	class Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		/// Set the existing value as the previous, new value as the current.
		virtual void Do() = 0;

		/// @brief Restores the target state saved before the command was applied.
		/// Set the previous value as the current.
		virtual void Undo() = 0;
	};

	class IntEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(int nextVal);

	public:
		/// @brief Initializes the previous and replacement integer values for undo/redo.
		/// @param valRef Value edited by the UI control.
		IntEditCommand(int& valRef);

	private:
		int& mValue;
		int	 mPrevValue;
		int	 mNextValue;
	};

	class ActorGroupEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(Common::ActorGroup nextVal);

	public:
		/// @brief Initializes the previous and replacement actor group for undo/redo.
		/// @param valRef Value edited by the UI control.
		ActorGroupEditCommand(Common::ActorGroup& valRef);

	private:
		Common::ActorGroup& mValue;
		Common::ActorGroup  mPrevValue;
		Common::ActorGroup  mNextValue;
	};

	class FloatEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(float nextVal);

	public:
		/// @brief Initializes the previous and replacement float values for undo/redo.
		/// @param valRef Value edited by the UI control.
		FloatEditCommand(float& valRef);

	private:
		float& mValue;
		float  mPrevValue;
		float  mNextValue;
	};

	class Vector2EditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(Math::FTVector2 nextVal);

	public:
		/// @brief Initializes the previous and replacement two-dimensional vectors for undo/redo.
		/// @param valRef Value edited by the UI control.
		Vector2EditCommand(Math::FTVector2& valRef);

	private:
		Math::FTVector2& mValue;
		Math::FTVector2	 mPrevValue;
		Math::FTVector2	 mNextValue;
	};

	class Vector3EditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Returns the prev val used by this vector3 edit command.
		/// @return Current prev val.
		Math::FTVector3 GetPrevVal();
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void			SetNextVal(Math::FTVector3 nextVal);

	public:
		/// @brief Initializes the previous and replacement three-dimensional vectors for undo/redo.
		/// @param valRef Value edited by the UI control.
		Vector3EditCommand(Math::FTVector3& valRef);

	private:
		Math::FTVector3& mValue;
		Math::FTVector3	 mPrevValue;
		Math::FTVector3	 mNextValue;
	};

	class Vector4EditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(Math::FTVector4 nextVal);

	public:
		/// @brief Initializes the previous and replacement four-dimensional vectors for undo/redo.
		/// @param valRef Value edited by the UI control.
		Vector4EditCommand(Math::FTVector4& valRef);

	private:
		Math::FTVector4& mValue;
		Math::FTVector4	 mPrevValue;
		Math::FTVector4	 mNextValue;
	};

	class StrEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(Common::FTDS::String nextVal);

	public:
		/// @brief Initializes the previous and replacement text values for undo/redo.
		/// @param valRef Value edited by the UI control.
		StrEditCommand(Common::FTDS::String& valRef);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~StrEditCommand();

	private:
		Common::FTDS::String& mValue;
		Common::FTDS::String	mPrevValue;
		Common::FTDS::String	mNextValue;
	};

	class WStrEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(std::wstring nextVal);

	public:
		/// @brief Initializes the previous and replacement wide strings for undo/redo.
		/// @param valRef Value edited by the UI control.
		WStrEditCommand(std::wstring& valRef);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~WStrEditCommand();

	private:
		std::wstring& mValue;
		std::wstring  mPrevValue;
		std::wstring  mNextValue;
	};

	class BoolEditCommand : public Command
	{
	public:
		/// @brief Applies the command's change to the target object.
		void Do() override;
		/// @brief Restores the target state saved before the command was applied.
		void Undo() override;

	public:
		/// @brief Updates the prev val used by subsequent operations.
		/// @param prevVal Replacement prev val.
		void SetPrevVal(bool prevVal);
		/// @brief Updates the next val used by subsequent operations.
		/// @param nextVal Replacement next val.
		void SetNextVal(bool nextVal);

	public:
		/// @brief Initializes the previous and replacement boolean values for undo/redo.
		/// @param valRef Value edited by the UI control.
		BoolEditCommand(bool& valRef);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~BoolEditCommand();

	private:
		bool& mValue;
		bool  mPrevValue;
		bool  mNextValue;
	};
} // namespace Editor
