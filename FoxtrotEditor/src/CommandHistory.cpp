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
// #include <imgui_stdlib.h>
#include <typeinfo>
#include <limits>

#include "Command.h"
#include "EditorLayer.h"
#include "ActorCommand.h"
#include "Core/TemplateFunctions.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/BufferSizes.h"

void CommandHistory::UndoCommand()
{
	if (mCurrent)
	{
		mCurrent->Undo();
		if (0 < mPrevious->GetSize())
		{
			mNext->Push(mCurrent);
			mCurrent = mPrevious->Peek();
			mPrevious->Pop();
		}
	}
}

void CommandHistory::RedoCommand()
{
	if (mCurrent)
	{
		mCurrent->Do();

		if (0 < mNext->GetSize())
		{
			mPrevious->Push(mCurrent);
			mCurrent = mNext->Peek();
			mNext->Pop();
		}
	}
}

void CommandHistory::ArrangeCommand()
{
	if (mCurrent)
		mPrevious->Push(mCurrent);
	if (!mNext->IsEmpty())
	{
		for (auto iter = mNext->Begin(); iter != mNext->End(); ++iter)
			delete *iter;
		mNext->Clear();
		mNext->Reserve(COMMAND_MAXCOUNT);
	}
}

void CommandHistory::Update()
{
	if (EditorLayer::GetInstance()->GetUndoKeyPressed())
		UndoCommand();
	if (EditorLayer::GetInstance()->GetRedoKeyPressed())
		RedoCommand();
}

void CommandHistory::UpdateVector2Value(const char* label, FTVector2& ref, float modSpeed)
{
	float vec2[2];
	vec2[0] = ref.x;
	vec2[1] = ref.y;

	if (ImGui::DragFloat2(label, vec2, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW Vector2EditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<Vector2EditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}
	ref.x = vec2[0];
	ref.y = vec2[1];
}

void CommandHistory::UpdateVector2Value(const char* label, DirectX::SimpleMath::Vector2& ref, float modSpeed)
{
	FTVector2 vec2 = FTVector2(ref.x, ref.y);
	UpdateVector2Value(label, vec2);
	ref = vec2.GetD3Vec2();
}

void CommandHistory::UpdateVector2Value(const char* label, b2Vec2& ref, float modSpeed)
{
	float vec2[2];
	vec2[0] = ref.x;
	vec2[1] = ref.y;

	if (ImGui::DragFloat2(label, vec2, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW B2Vec2EditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<B2Vec2EditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}

	ref.x = vec2[0];
	ref.y = vec2[1];
}

void CommandHistory::UpdateVector3Value(const char* label, FTVector3& ref, float modSpeed)
{
	float vec3[3];
	vec3[0] = ref.x;
	vec3[1] = ref.y;
	vec3[2] = ref.z;

	if (ImGui::DragFloat3(label, vec3, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW Vector3EditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<Vector3EditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}

	ref.x = vec3[0];
	ref.y = vec3[1];
	ref.z = vec3[2];
}

void CommandHistory::UpdateVector3Value(const char* label, DirectX::SimpleMath::Vector3& ref, float modSpeed)
{
	float vec3[3];
	vec3[0] = ref.x;
	vec3[1] = ref.y;
	vec3[2] = ref.z;

	if (ImGui::DragFloat3(label, vec3, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW DXVector3EditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<DXVector3EditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}

	ref.x = vec3[0];
	ref.y = vec3[1];
	ref.z = vec3[2];
}

void CommandHistory::UpdateVector4Value(const char* label, FTVector4& ref, float modSpeed)
{
	float vec4[4];
	vec4[0] = ref.x;
	vec4[1] = ref.y;
	vec4[2] = ref.z;
	vec4[3] = ref.a;

	if (ImGui::DragFloat4(label, vec4, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW Vector4EditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<Vector4EditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}

	ref.x = vec4[0];
	ref.y = vec4[1];
	ref.z = vec4[2];
	ref.a = vec4[3];
}

void CommandHistory::UpdateStringValue(const char* label, FTDS::String& ref)
{
	if (ref.Capacity() < BufferSize::STRING_BUFFER_SIZE)
		ref.Reserve(BufferSize::STRING_BUFFER_SIZE);

	// static StrEditCommand* command;

	static char strVal[BufferSize::STRING_BUFFER_SIZE] = { 0 };
	strcpy_s(strVal, ref.C_Str());

	if (ImGui::InputText(label, strVal, BufferSize::STRING_BUFFER_SIZE, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		StrEditCommand* command = DBG_NEW StrEditCommand(ref);
		ref.Assign(strVal);
		command->SetNextVal(ref);

		command = nullptr;
	}
	strVal[0] = '\0';
}

void CommandHistory::UpdateStateValue(const char* label, Actor::State& state)
{
	bool isActive = false;

	if (ImGui::Checkbox(label, &isActive))
	{
		mCurrent = DBG_NEW ActorStateEditCommand(state);

		if (isActive)
			state = Actor::State::EActive;
		else
			state = Actor::State::EDead;

		static_cast<ActorStateEditCommand*>(mCurrent)->SetNextVal(state);
	}
}

void CommandHistory::UpdateFloatValue(const char* label, float& ref, float modSpeed)
{
	if (ImGui::DragFloat(label, &ref, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW FloatEditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<FloatEditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}
}

void CommandHistory::UpdateIntValue(const char* label, int& ref, int modSpeed)
{
	if (ImGui::DragInt(label, &ref, modSpeed))
	{
		if (!mIsRecording)
		{
			mIsRecording = true;
			ArrangeCommand();
			mCurrent = DBG_NEW IntEditCommand(ref);
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (mCurrent)
		{
			mIsRecording = false;
			static_cast<IntEditCommand*>(mCurrent)->SetNextVal(ref);
		}
	}
}

void CommandHistory::UpdateIntValue(const char* label, int& ref, int min, int max, int modSpeed)
{
	Math::Clamp(ref, min, max);
	UpdateIntValue(label, ref, modSpeed);
}

void CommandHistory::UpdateBoolValue(const char* label, bool& ref)
{
	bool updated = ref;

	if (ImGui::Checkbox(label, &ref))
	{
		mCurrent = DBG_NEW BoolEditCommand(updated);
		static_cast<BoolEditCommand*>(mCurrent)->SetNextVal(ref);
	}
}

void CommandHistory::UpdateUnsignedIntValue(const char* label, UINT& ref, UINT modSpeed)
{
	int val = static_cast<int>(ref);
	UpdateIntValue(label, val);
	ref = static_cast<UINT>(val);
}

void CommandHistory::ShutDown()
{
	for (auto iter = mPrevious->Begin(); iter != mPrevious->End(); ++iter)
		delete *iter;
	for (auto iter = mNext->Begin(); iter != mNext->End(); ++iter)
		delete *iter;
	delete mCurrent;

	delete mPrevious;
	delete mNext;
}

Command* CommandHistory::GetCurrentCommand()
{
	return mCurrent;
}

CommandHistory::CommandHistory()
	: mCurrent(nullptr)
	, mPrevious(DBG_NEW FTDS::ArrayStack<Command*>(COMMAND_MAXCOUNT))
	, mNext(DBG_NEW FTDS::ArrayStack<Command*>(COMMAND_MAXCOUNT))
	, mIsRecording(false)
{
}

CommandHistory::~CommandHistory()
{
}