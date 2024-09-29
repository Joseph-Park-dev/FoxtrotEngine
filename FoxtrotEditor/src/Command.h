#pragma once
#include <list>

#include "directxtk/SimpleMath.h"

#include "Math/FTMath.h"
#include "Actors/Actor.h"

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

class IntEditCommandPtr : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	IntEditCommandPtr(int* valRef, int nextVal);

private:
	int* mValue;
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
	Vector2EditCommand(FTVector2& valRef, FTVector2 nextVal);

private:
	FTVector2& mValue;
	FTVector2 mPrevValue;
	FTVector2 mNextValue;
};

class Vector3EditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	Vector3EditCommand(FTVector3& valRef, FTVector3 nextVal);

private:
	FTVector3& mValue;
	FTVector3 mPrevValue;
	FTVector3 mNextValue;
};

class DXVector3EditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	DXVector3EditCommand(DirectX::SimpleMath::Vector3& valRef, DirectX::SimpleMath::Vector3 nextVal);

private:
	DirectX::SimpleMath::Vector3& mValue;
	DirectX::SimpleMath::Vector3 mPrevValue;
	DirectX::SimpleMath::Vector3 mNextValue;
};

class StrEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	StrEditCommand(std::string& valRef, std::string nextVal);
	~StrEditCommand();

private:
	std::string&  mValue;
	std::string   mPrevValue;
	std::string   mNextValue;
};

class WStrEditCommand : public Command
{
public:
	void Execute() override;
	void Undo() override;

public:
	WStrEditCommand(std::wstring& valRef, std::wstring nextVal);
	~WStrEditCommand();

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