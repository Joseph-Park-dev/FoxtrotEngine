#include "CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <typeinfo>

#include "Command.h"
#include "EditorLayer.h"
#include "Core/TemplateFunctions.h"

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
		LogString("undo");
	}
	if (EditorLayer::GetInstance()->GetRedoKeyPressed())
	{
		RedoCommand();
		LogString("redo");
	}
}

void CommandHistory::UpdateVector2Value(std::string label, FTVector2& ref, float modSpeed)
{
	FTVector2 updatedVal = ref;
	float* vec2 = new float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = GetIsRecording();
	if (ImGui::DragFloat2(label.c_str(), vec2, modSpeed)) {
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
			SetIsRecording(true);
			AddCommand(new Vector2EditCommand(ref, updatedVal));
		}
		updatedVal = FTVector2(vec2[0], vec2[1]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(new Vector2EditCommand(ref, updatedVal));
			SetIsRecording(false);
		}
	}
	delete[] vec2;
}

void CommandHistory::UpdateVector3Value(std::string label, FTVector3& ref, float modSpeed)
{
	FTVector3 updatedVal = ref;
	float* vec3 = new float[3];
	vec3[0] = updatedVal.x;
	vec3[1] = updatedVal.y;
	vec3[2] = updatedVal.z;
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed)){
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)){
			SetIsRecording(true);
			AddCommand(new Vector3EditCommand(ref, updatedVal));
		}
		updatedVal = FTVector3(vec3[0], vec3[1], vec3[2]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(new Vector3EditCommand(ref, updatedVal));
			SetIsRecording(false);
		}
	}
	delete[] vec3;
}

void CommandHistory::UpdateVector3Value(std::string label, DirectX::SimpleMath::Vector3& ref, float modSpeed)
{
	DirectX::SimpleMath::Vector3 updatedVal = ref;
	float* vec3 = new float[3];
	vec3[0] = updatedVal.x;
	vec3[1] = updatedVal.y;
	vec3[2] = updatedVal.z;
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed)) {
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
			SetIsRecording(true);
			AddCommand(new DXVector3EditCommand(ref, updatedVal));
		}
		updatedVal = DirectX::SimpleMath::Vector3(vec3[0], vec3[1], vec3[2]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(new DXVector3EditCommand(ref, updatedVal));
			SetIsRecording(false);
		}
	}
	delete[] vec3;
}

void CommandHistory::UpdateFloatValue(std::string label, float* ref, float modSpeed)
{
	float prevFloat = *ref;
	float* rotationBuf = new float(prevFloat);
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat(label.c_str(), rotationBuf, modSpeed))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new FloatEditCommand(ref, prevFloat));
		}
		*ref = *rotationBuf;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new FloatEditCommand(ref, prevFloat));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete rotationBuf;
}

void CommandHistory::UpdateFloatValue(std::string label, float* ref, float val, float modSpeed, Transform* transf, TransSetFloatFunc setFunc)
{
	float prevRotation = val;
	float* rotationBuf = new float(prevRotation);
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat(label.c_str(), rotationBuf, modSpeed))
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

void CommandHistory::UpdateIntValue(std::string label, int* ref, int modSpeed)
{
	int prevInt = *ref;
	int* intBuf = new int(prevInt);
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragInt(label.c_str(), intBuf, modSpeed))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new IntEditCommandPtr(ref, prevInt));
		}
		*ref = *intBuf;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new IntEditCommandPtr(ref, prevInt));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete intBuf;
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
