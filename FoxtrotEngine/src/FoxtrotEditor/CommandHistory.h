#pragma once
#ifdef _DEBUG
#include <deque>
#include <functional>

#include "FoxtrotEngine/Core/SingletonMacro.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEditor/Command.h"

#define COMMAND_MAXCOUNT 30
#define FLOATMOD_SPEED 0.01f
#define INTMOD_SPEED 1
using TransSetFloatFunc = std::function<void(Transform*, float)>;

class CommandHistory
{
	SINGLETON(CommandHistory)

public:
	void AddCommand(Command* command);
	void UndoCommand();
	void RedoCommand();

public:
	void StartCMDRecord();
	void EndCMDRecord();
	void MergeCMDRecord();
	void Update();

	void UpdateVector3Value(std::string label, FTVector3* ref, FTVector3 val, float modSpeed);
	void UpdateFloatValue(std::string label, float* ref, float modSpeed);
	void UpdateFloatValue(std::string label, float* ref, float val, float modSpeed, Transform* transf, TransSetFloatFunc setFunc);
	void UpdateIntValue(std::string label, int* ref, int modSpeed);

	bool GetIsRecording() { return mRecording; }
	void SetIsRecording(bool val) { mRecording = val; }

private:
	std::deque<Command*> mCommandDeq;
	int  mCommandPointer;
	bool mRecording;
	int	 mCMDStartPointer;
	int  mCMDEndPointer;

private:
	Command* QueryCommand();
};
#endif