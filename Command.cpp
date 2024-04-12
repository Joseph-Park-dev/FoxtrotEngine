#ifdef _DEBUG
#include <string>

#include "Command.h"
#include "Math.h"
#include "TemplateFunctions.h"

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

void FloatEditCommand::Execute()
{
	mPrevValue = *mValue;
	*mValue = mNextValue;
	LogFloat("Previous Val", mPrevValue);
	LogFloat("Current Val", *mValue);
	LogFloat("Next Val", mNextValue);
}

void FloatEditCommand::Undo()
{
	float nextVal = *mValue;
	*mValue = mPrevValue;
	mNextValue = nextVal;
	LogFloat("Previous Val", mPrevValue);
	LogFloat("Current Val", *mValue);
	LogFloat("Next Val", mNextValue);
}

FloatEditCommand::FloatEditCommand(float* valRef, float nextVal)
	: mPrevValue(), mValue(valRef), mNextValue(nextVal)
{
}

void Vector2EditCommand::Execute()
{
	mPrevValue = *mValue;
	*mValue = mNextValue;
	LogVector2("Previous Val", mPrevValue);
	LogVector2("Current Val", *mValue);
	LogVector2("Next Val", mNextValue);
}

void Vector2EditCommand::Undo()
{
	Vector2 nextVal = *mValue;
	*mValue = mPrevValue;
	mNextValue = nextVal;
	LogVector2("Previous Val", mPrevValue);
	LogVector2("Current Val", *mValue);
	LogVector2("Next Val", mNextValue);
}

Vector2EditCommand::Vector2EditCommand(Vector2* valRef, Vector2 nextVal)
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

StrEditCommand::StrEditCommand(std::wstring& valRef, std::wstring nextVal)
	: mPrevValue()
	, mValue	(valRef)
	, mNextValue(nextVal)
{}

StrEditCommand::~StrEditCommand()
{}

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
#endif // _DEBUG