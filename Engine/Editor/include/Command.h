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
		/// Set the existing value as the previous, new value as the current.
		virtual void Do() = 0;

		/// Set the previous value as the current.
		virtual void Undo() = 0;
	};

	class IntEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(int nextVal);

	public:
		IntEditCommand(int& valRef);

	private:
		int& mValue;
		int	 mPrevValue;
		int	 mNextValue;
	};

	class ActorGroupEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(Core::ActorGroup nextVal);

	public:
		ActorGroupEditCommand(Core::ActorGroup& valRef);

	private:
		Core::ActorGroup& mValue;
		Core::ActorGroup  mPrevValue;
		Core::ActorGroup  mNextValue;
	};

	class FloatEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(float nextVal);

	public:
		FloatEditCommand(float& valRef);

	private:
		float& mValue;
		float  mPrevValue;
		float  mNextValue;
	};

	class Vector2EditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(Math::FTVector2 nextVal);

	public:
		Vector2EditCommand(Math::FTVector2& valRef);

	private:
		Math::FTVector2& mValue;
		Math::FTVector2	 mPrevValue;
		Math::FTVector2	 mNextValue;
	};

	class Vector3EditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		Math::FTVector3 GetPrevVal();
		void			SetNextVal(Math::FTVector3 nextVal);

	public:
		Vector3EditCommand(Math::FTVector3& valRef);

	private:
		Math::FTVector3& mValue;
		Math::FTVector3	 mPrevValue;
		Math::FTVector3	 mNextValue;
	};

	class Vector4EditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(Math::FTVector4 nextVal);

	public:
		Vector4EditCommand(Math::FTVector4& valRef);

	private:
		Math::FTVector4& mValue;
		Math::FTVector4	 mPrevValue;
		Math::FTVector4	 mNextValue;
	};

	class StrEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(Common::FTDS::String nextVal);

	public:
		StrEditCommand(Common::FTDS::String& valRef);
		~StrEditCommand();

	private:
		Common::FTDS::String& mValue;
		Common::FTDS::String	mPrevValue;
		Common::FTDS::String	mNextValue;
	};

	class WStrEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetNextVal(std::wstring nextVal);

	public:
		WStrEditCommand(std::wstring& valRef);
		~WStrEditCommand();

	private:
		std::wstring& mValue;
		std::wstring  mPrevValue;
		std::wstring  mNextValue;
	};

	class BoolEditCommand : public Command
	{
	public:
		void Do() override;
		void Undo() override;

	public:
		void SetPrevVal(bool prevVal);
		void SetNextVal(bool nextVal);

	public:
		BoolEditCommand(bool& valRef);
		~BoolEditCommand();

	private:
		bool& mValue;
		bool  mPrevValue;
		bool  mNextValue;
	};
} // namespace Editor
