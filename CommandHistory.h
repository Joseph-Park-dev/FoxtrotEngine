#pragma once
#include <deque>
#include <functional>

#include "SingletonMacro.h"
#include "Command.h"

#define COMMAND_MAXCOUNT 30
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

	void UpdateVectorValue(std::string label, FTVector2* ref, FTVector2 val);
	void UpdateFloatValue(std::string label, float* ref, float val, Transform* transf, TransSetFloatFunc setFunc);

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