// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <typeinfo>

#include "Command.h"
#include "EditorLayer.h"
#include "Core/TemplateFunctions.h"
#include "Debugging/DebugMemAlloc.h"

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
			LogInt(mCommandPointer);
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
		LogInt(mCommandPointer);
		Command* cmd = QueryCommand();
		if (cmd)
			cmd->Execute();
	}
}

void CommandHistory::StartCMDRecord()
{
	if (!mIsRecording)
	{
		mIsRecording = true;
		mCMDStartPointer = mCommandPointer;
	}
}

void CommandHistory::EndCMDRecord()
{
	if (mIsRecording)
	{
		mIsRecording = false;
		mCMDEndPointer = mCommandPointer;
		MergeCMDRecord();
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
	float* vec2 = DBG_NEW float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = mIsRecording;
	if (ImGui::DragFloat2(label.c_str(), vec2, modSpeed)) {
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
			mIsRecording = true;
			AddCommand(DBG_NEW Vector2EditCommand(ref, updatedVal));
		}
		updatedVal = FTVector2(vec2[0], vec2[1]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(DBG_NEW Vector2EditCommand(ref, updatedVal));
			mIsRecording = false;
		}
	}
	delete[] vec2;
}

void CommandHistory::UpdateVector2Value(std::string label, b2Vec2& ref, float modSpeed)
{
	b2Vec2 updatedVal = ref;
	float* vec2 = DBG_NEW float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = mIsRecording;
	if (ImGui::DragFloat2(label.c_str(), vec2, modSpeed)) {
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
			mIsRecording = true;
			AddCommand(DBG_NEW B2Vec2EditCommand(ref, updatedVal));
		}
		updatedVal.x = vec2[0];
		updatedVal.y = vec2[1];
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(DBG_NEW B2Vec2EditCommand(ref, updatedVal));
			mIsRecording = false;
		}
	}
	delete[] vec2;
}

void CommandHistory::UpdateVector3Value(std::string label, FTVector3& ref, float modSpeed)
{
	FTVector3 updatedVal = ref;
	float* vec3 = DBG_NEW float[3];
	vec3[0] = updatedVal.x;
	vec3[1] = updatedVal.y;
	vec3[2] = updatedVal.z;
	bool isRecording = mIsRecording;
	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed)){
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)){
			mIsRecording = true;
			AddCommand(DBG_NEW Vector3EditCommand(ref, updatedVal));
		}
		updatedVal = FTVector3(vec3[0], vec3[1], vec3[2]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(DBG_NEW Vector3EditCommand(ref, updatedVal));
			mIsRecording = false;
			LogInt(mCommandDeq.size());
		}
	}
	delete[] vec3;
}

void CommandHistory::UpdateVector3Value(std::string label, DirectX::SimpleMath::Vector3& ref, float modSpeed)
{
	DirectX::SimpleMath::Vector3 updatedVal = ref;
	float* vec3 = DBG_NEW float[3];
	vec3[0] = updatedVal.x;
	vec3[1] = updatedVal.y;
	vec3[2] = updatedVal.z;
	bool isRecording = mIsRecording;
	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed)) {
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left)) {
			mIsRecording = true;
			AddCommand(DBG_NEW DXVector3EditCommand(ref, updatedVal));
		}
		updatedVal = DirectX::SimpleMath::Vector3(vec3[0], vec3[1], vec3[2]);
		ref = updatedVal;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			AddCommand(DBG_NEW DXVector3EditCommand(ref, updatedVal));
			mIsRecording = false;
		}
	}
	delete[] vec3;
}

void CommandHistory::UpdateFloatValue(std::string label, float* ref, float modSpeed)
{
	float prevFloat = *ref;
	float* rotationBuf = DBG_NEW float(prevFloat);
	bool isRecording = mIsRecording;
	if (ImGui::DragFloat(label.c_str(), rotationBuf, modSpeed))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			mIsRecording = true;
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW FloatEditCommand(ref, prevFloat));
		}
		*ref = *rotationBuf;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW FloatEditCommand(ref, prevFloat));
			mIsRecording = false;
		}
	}
	delete rotationBuf;
}

void CommandHistory::UpdateIntValue(std::string label, int* ref, int modSpeed)
{
	int prevInt = *ref;
	int* intBuf = DBG_NEW int(prevInt);
	bool isRecording = mIsRecording;
	if (ImGui::DragInt(label.c_str(), intBuf, modSpeed))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			mIsRecording = true;
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW IntEditCommandPtr(ref, prevInt));
		}
		*ref = *intBuf;
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW IntEditCommandPtr(ref, prevInt));
			mIsRecording = false;
		}
	}
	delete intBuf;
}

void CommandHistory::UpdateBoolValue(std::string label, bool& ref)
{
	bool valToUpdate = ref;
	bool isRecording = mIsRecording;
	if (ImGui::Checkbox(label.c_str(), &valToUpdate))
	{
		if (valToUpdate != ref)
		{
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW BoolEditCommand(ref, valToUpdate));
			ref = valToUpdate;
		}
	}
}

void CommandHistory::ShutDown()
{
	if (!mCommandDeq.empty())
		for (size_t i = 0; i < mCommandDeq.size(); ++i)
			delete mCommandDeq.at(i);
	mCommandDeq.clear();
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
	, mCMDEndPointer(0)
	, mIsRecording(false)
{}

CommandHistory::~CommandHistory()
{}