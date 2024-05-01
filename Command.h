#ifdef _DEBUG
#pragma once
#include <list>

#include "Math.h"
#include "Actor.h"

class Command;
class FTVector2;

class Command
{
public:
	virtual void Execute() {};
	virtual void Undo() {};

public:
	Command();
};

class IntEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	IntEditCommand(int& valRef, int nextVal);

private:
	int& mValue;
	int mPrevValue;
	int mNextValue;
};

class FloatEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	FloatEditCommand(float* valRef, float nextVal);

private:
	float* mValue;
	float mPrevValue;
	float mNextValue;
};

class Vector2EditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	Vector2EditCommand(FTVector2* valRef, FTVector2 nextVal);

private:
	FTVector2* mValue;
	FTVector2 mPrevValue;
	FTVector2 mNextValue;
};

class StrEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	StrEditCommand(std::wstring& valRef, std::wstring nextVal);
	~StrEditCommand();

private:
	std::wstring&  mValue;
	std::wstring   mPrevValue;
	std::wstring   mNextValue;
};

class BoolEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	BoolEditCommand(bool& valRef, bool nextVal);
	~BoolEditCommand();

private:
	bool& mValue;
	bool  mPrevValue;
	bool  mNextValue;
};

class StateEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	StateEditCommand(Actor::State& valRef, Actor::State nextVal);

private:
	Actor::State& mValue;
	Actor::State  mPrevValue;
	Actor::State  mNextValue;
};
#endif // _DEBUG