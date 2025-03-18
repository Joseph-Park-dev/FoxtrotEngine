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
#include <list>

#include "directxtk/SimpleMath.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"

class Command
{
public:
	/// Set the existing value as the previous, new value as the current.
	virtual void Do() = 0;

	/// Set the previous value as the current.
	virtual void Undo() = 0;

public:
	Command();
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

class B2Vec2EditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(b2Vec2 nextVal);

public:
	B2Vec2EditCommand(b2Vec2& valRef);

private:
	b2Vec2& mValue;
	b2Vec2	mPrevValue;
	b2Vec2	mNextValue;
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

class DXVector3EditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(DirectX::SimpleMath::Vector3 nextVal);

public:
	DXVector3EditCommand(DirectX::SimpleMath::Vector3& valRef);

private:
	DirectX::SimpleMath::Vector3& mValue;
	DirectX::SimpleMath::Vector3  mPrevValue;
	DirectX::SimpleMath::Vector3  mNextValue;
};

class StrEditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(std::string nextVal);

public:
	StrEditCommand(std::string& valRef);
	~StrEditCommand();

private:
	std::string& mValue;
	std::string	 mPrevValue;
	std::string	 mNextValue;
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

class ActorStateEditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(Actor::State nextVal);

public:
	ActorStateEditCommand(Actor::State& valRef);

private:
	Actor::State& mValue;
	Actor::State  mPrevValue;
	Actor::State  mNextValue;
};

class BoolEditCommand : public Command
{
public:
	void Do() override;
	void Undo() override;

public:
	void SetNextVal(bool nextVal);

public:
	BoolEditCommand(bool& valRef);
	~BoolEditCommand();

private:
	bool& mValue;
	bool  mPrevValue;
	bool  mNextValue;
};