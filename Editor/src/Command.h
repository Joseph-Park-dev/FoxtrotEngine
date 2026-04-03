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
#include "Math/FTMath.h"
#include "FTDS/Static/FTString.h"

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
	void SetNextVal(ActorGroup nextVal);

public:
	ActorGroupEditCommand(ActorGroup& valRef);

private:
	ActorGroup& mValue;
	ActorGroup	mPrevValue;
	ActorGroup	mNextValue;
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
	void SetNextVal(FTVector2 nextVal);

public:
	Vector2EditCommand(FTVector2& valRef);

private:
	FTVector2& mValue;
	FTVector2  mPrevValue;
	FTVector2  mNextValue;
};

class Vector3EditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	FTVector3 GetPrevVal();
	void	  SetNextVal(FTVector3 nextVal);

public:
	Vector3EditCommand(FTVector3& valRef);

private:
	FTVector3& mValue;
	FTVector3  mPrevValue;
	FTVector3  mNextValue;
};

class Vector4EditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(FTVector4 nextVal);

public:
	Vector4EditCommand(FTVector4& valRef);

private:
	FTVector4& mValue;
	FTVector4  mPrevValue;
	FTVector4  mNextValue;
};

class StrEditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(FTDS::String nextVal);

public:
	StrEditCommand(FTDS::String& valRef);
	~StrEditCommand();

private:
	FTDS::String& mValue;
	FTDS::String  mPrevValue;
	FTDS::String  mNextValue;
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