#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <typeinfo>

#include "CommandHistory.h"
#include "Command.h"
#include "TemplateFunctions.h"
#include "EditorLayer.h"

void CommandHistory::AddCommand(Command* command)
{
	if (mCommandDeq.size() < COMMAND_MAXCOUNT)
	{
		command->Execute();
		LogInt("Executed! : ", mCommandPointer);
		int endIndex = mCommandDeq.size() - 1;
		int dstFromEnd = endIndex - mCommandPointer;
		if (0 < dstFromEnd)
		{
			int insertPoint = mCommandPointer + 1;
			mCommandDeq.insert(mCommandDeq.begin() + insertPoint, command);
			int popCount = mCommandDeq.size() - 1 - insertPoint;
			for (int i = 0; i < popCount; ++i)
				mCommandDeq.pop_back();
		}
		else
		{
			mCommandDeq.emplace_back(command);
		}
		mCommandPointer = mCommandDeq.size() - 1;
	}
}

void CommandHistory::UndoCommand()
{
	if (0 < mCommandPointer)
	{
		Command* cmd = QueryCommand();
		if (cmd)
		{
			cmd->Undo();
			--mCommandPointer;
		}
	}
}

void CommandHistory::RedoCommand()
{
	if (mCommandPointer < mCommandDeq.size() - 1)
	{
		++mCommandPointer;
		Command* cmd = QueryCommand();
		if (cmd)
		{
			cmd->Execute();
		}
	}
}

void CommandHistory::StartCMDRecord()
{
	if (!mRecording)
	{
		mRecording = true;
		mCMDStartPointer = mCommandPointer;
	}
}

void CommandHistory::EndCMDRecord()
{
	if (mRecording)
	{
		mRecording = false;
		mCMDEndPointer = mCommandPointer;
		//MergeCMDRecord();
	}
}

void CommandHistory::MergeCMDRecord()
{
	if (0 < mCMDEndPointer - mCMDStartPointer)
		if (mCommandDeq[mCMDStartPointer] != nullptr
			&& mCommandDeq[mCMDEndPointer] != nullptr)
		{
			mCommandDeq[mCMDStartPointer + 1] = mCommandDeq[mCMDEndPointer];
			int popCount = mCMDEndPointer - mCMDStartPointer - 1;
			for (size_t i = 0; i < popCount; ++i)
				mCommandDeq.pop_back();
		}
}

void CommandHistory::Update()
{
	if (EditorLayer::GetInstance()->GetUndoKeyPressed())
	{
		UndoCommand();
		printf("pointer : %d, size : %d", mCommandPointer, mCommandDeq.size());
	}
	if (EditorLayer::GetInstance()->GetRedoKeyPressed())
	{
		RedoCommand();
		printf("pointer : %d, size : %d", mCommandPointer, mCommandDeq.size());
	}
}

void CommandHistory::UpdateVectorValue(std::string label, Vector2* ref, Vector2 val)
{
	Vector2 updatedVal = val;
	float* vec2 = new float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat2(label.c_str(), vec2))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(ref, val));
		}
		updatedVal = Vector2(vec2[0], vec2[1]);
		*ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(ref, updatedVal));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete[] vec2;
}

void CommandHistory::UpdateFloatValue(std::string label, float* ref, float val, Transform* transf, TransSetFloatFunc setFunc)
{
	float prevRotation = val;
	float* rotationBuf = new float(prevRotation);
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat(label.c_str(), rotationBuf))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new FloatEditCommand(ref, prevRotation));
		}
		setFunc(transf, *rotationBuf);
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new FloatEditCommand(ref, val));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete rotationBuf;
}

Command* CommandHistory::QueryCommand()
{
	if(!mCommandDeq.empty())
		return mCommandDeq[mCommandPointer];
	return nullptr;
}

CommandHistory::CommandHistory()
	: mCommandDeq{}
	, mCommandPointer(0) // Pointer value becomes 0 when the 1st element is inserted
	, mCMDStartPointer(0)
	, mRecording(false)
{}

CommandHistory::~CommandHistory()
{}
#endif // _DEBUG