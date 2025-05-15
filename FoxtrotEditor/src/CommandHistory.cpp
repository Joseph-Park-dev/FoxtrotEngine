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
#include <imgui_stdlib.h>
#include <typeinfo>

#include "Command.h"
#include "EditorLayer.h"
#include "ActorCommand.h"
#include "Core/TemplateFunctions.h"
#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/BufferSizes.h"

void CommandHistory::AddCommand(Command* command)
{
	if (mCommandDeq.size() < COMMAND_MAXCOUNT)
	{
		if (0 < mCommandDeq.size())
		{
			size_t endIdx = mCommandDeq.size() - 1;
			if (0 < endIdx)
			{
				size_t distFromEnd = endIdx - mCommandPointer;
				if (0 < distFromEnd)
				{
					for (size_t i = 0; i < distFromEnd; ++i)
					{
						delete mCommandDeq.back();
						mCommandDeq.pop_back();
					}
				}
			}
		}
		mCommandDeq.push_back(command);
		mCommandPointer = mCommandDeq.size() - 1;
	}
}

void CommandHistory::UndoCommand()
{
	if (0 <= mCommandPointer)
	{
		Command* cmd = GetCurrentCommand();
		if (cmd)
		{
			LogInt(mCommandPointer);
			cmd->Undo();
			if (0 < mCommandPointer)
				--mCommandPointer;
		}
	}
}

void CommandHistory::RedoCommand()
{
	if (mCommandPointer < mCommandDeq.size())
	{
		Command* cmd = GetCurrentCommand();
		if (cmd)
		{
			LogInt(mCommandPointer);
			cmd->Do();
			if (mCommandPointer < mCommandDeq.size() - 1)
				++mCommandPointer;
		}
	}
}

void CommandHistory::StartCMDRecord()
{
	if (!mIsRecording)
	{
		mIsRecording	 = true;
		mCMDStartPointer = mCommandPointer;
	}
}

void CommandHistory::EndCMDRecord()
{
	if (mIsRecording)
	{
		mIsRecording   = false;
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
			int popCount					  = mCMDEndPointer - mCMDStartPointer - 1;
			for (size_t i = 0; i < popCount; ++i)
				mCommandDeq.pop_back();
		}
}

void CommandHistory::Update()
{
	if (EditorLayer::GetInstance()->GetUndoKeyPressed())
		UndoCommand();
	if (EditorLayer::GetInstance()->GetRedoKeyPressed())
		RedoCommand();
}

void CommandHistory::UpdateVector2Value(std::string label, FTVector2& ref, float modSpeed)
{
	static Vector2EditCommand* command;

	float vec2[2];
	vec2[0] = ref.x;
	vec2[1] = ref.y;

	if (ImGui::DragFloat2(label.c_str(), vec2, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command		 = DBG_NEW Vector2EditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording = false;
				command->SetNextVal(FTVector2(vec2[0], vec2[1]));
				AddCommand(command);
				command = nullptr;
			}
		}
	}
	ref.x = vec2[0];
	ref.y = vec2[1];
}

void CommandHistory::UpdateVector2Value(std::string label, b2Vec2& ref, float modSpeed)
{
	static B2Vec2EditCommand* command;

	float vec2[2];
	vec2[0] = ref.x;
	vec2[1] = ref.y;

	if (ImGui::DragFloat2(label.c_str(), vec2, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command		 = DBG_NEW B2Vec2EditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording   = false;
				b2Vec2 updated = b2Vec2_zero;
				updated.x	   = vec2[0];
				updated.y	   = vec2[1];
				command->SetNextVal(updated);
				AddCommand(command);
				command = nullptr;
			}
		}
	}
	ref.x = vec2[0];
	ref.y = vec2[1];
}

void CommandHistory::UpdateVector3Value(std::string label, FTVector3& ref, float modSpeed)
{
	static Vector3EditCommand* command;

	float vec3[3];
	vec3[0] = ref.x;
	vec3[1] = ref.y;
	vec3[2] = ref.z;

	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command		 = DBG_NEW Vector3EditCommand(ref);
			}
		}
	}

	if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
	{
		if (command)
		{
			mIsRecording = false;
			command->SetNextVal(ref);
			AddCommand(command);
			command = nullptr;
		}
	}

	ref.x = vec3[0];
	ref.y = vec3[1];
	ref.z = vec3[2];
}

void CommandHistory::UpdateVector3Value(std::string label, DirectX::SimpleMath::Vector3& ref, float modSpeed)
{
	static DXVector3EditCommand* command;

	float vec3[3];
	vec3[0] = ref.x;
	vec3[1] = ref.y;
	vec3[2] = ref.z;

	if (ImGui::DragFloat3(label.c_str(), vec3, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command		 = DBG_NEW DXVector3EditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording = false;
				command->SetNextVal(ref);
				AddCommand(command);
				command = nullptr;
			}
		}
	}
	ref.x = vec3[0];
	ref.y = vec3[1];
	ref.z = vec3[2];
}

void CommandHistory::UpdateStringValue(std::string label, std::string& ref)
{
	if (ref.capacity() < BufferSize::STRING_BUFFER_SIZE)
		ref.reserve(BufferSize::STRING_BUFFER_SIZE);

	static StrEditCommand* command;

	if (ImGui::InputText(label.c_str(), &ref))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command = DBG_NEW StrEditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording = false;
				command->SetNextVal(ref);
				AddCommand(command);
				command = nullptr;
			}
		}
	}
}

void CommandHistory::UpdateStateValue(std::string label, Actor::State& state)
{
	static ActorStateEditCommand* command = nullptr;

	bool isActive = false;

	if (ImGui::Checkbox(label.c_str(), &isActive))
	{
		command = DBG_NEW ActorStateEditCommand(state);

		if (isActive)
			state = Actor::State::EActive;
		else
			state = Actor::State::EDead;

		command->SetNextVal(state);
		CommandHistory::GetInstance()->AddCommand(command);
	}
}

void CommandHistory::UpdateActorAddition(EditorElement* editorElement)
{
	ActorAdditionCommand* command = nullptr;

	// ActorCommand has been triggered after program initialization.
	// Thus the latest ActorCommand is not null.
	if (CommandHistory::GetInstance()->GetLatestActorCommand())
	{
		// Set the input as the next value of the latest ActorAddition Command
		CommandHistory::GetInstance()->GetLatestActorCommand()->SetNextVal(editorElement);
	}
	// Set the latest Actor Addition Command
	command = DBG_NEW ActorAdditionCommand(editorElement);
	if (command)
	{
		CommandHistory::GetInstance()->SetLatestActorCommand(command);
		CommandHistory::GetInstance()->AddCommand(command);
	}
}

ActorCommand* CommandHistory::GetLatestActorCommand() { return mLatestActorCommand; }
void		  CommandHistory::SetLatestActorCommand(ActorCommand* command) { mLatestActorCommand = command; }

void CommandHistory::UpdateFloatValue(std::string label, float& ref, float modSpeed)
{
	static FloatEditCommand* command;

	if (ImGui::DragFloat(label.c_str(), &ref, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command		 = DBG_NEW FloatEditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording = false;
				command->SetNextVal(ref);
				AddCommand(command);
				command = nullptr;
			}
		}
	}
}

void CommandHistory::UpdateIntValue(std::string label, int& ref, int modSpeed)
{
	static IntEditCommand* command;

	std::string newLabel = "##" + label;
	if (ImGui::DragInt(newLabel.c_str(), &ref, modSpeed))
	{
		if (!mIsRecording)
		{
			if (!command)
			{
				mIsRecording = true;
				command = DBG_NEW IntEditCommand(ref);
			}
		}
	}
	else
	{
		if (mIsRecording && ImGui::IsItemDeactivatedAfterEdit())
		{
			if (command)
			{
				mIsRecording = false;
				command->SetNextVal(ref);
				AddCommand(command);
				command = nullptr;
			}
		}
	}
}

void CommandHistory::UpdateIntValue(std::string label, int& ref, int min, int max, int modSpeed)
{
	Math::Clamp(ref, min, max);
	UpdateIntValue(label, ref, modSpeed);
}

void CommandHistory::UpdateBoolValue(std::string label, bool& ref)
{
	static BoolEditCommand* command = nullptr;
	bool					updated = ref;

	if (ImGui::Checkbox(label.c_str(), &ref))
	{
		command = DBG_NEW BoolEditCommand(updated);
		command->SetNextVal(ref);
		CommandHistory::GetInstance()->AddCommand(command);
	}
}

void CommandHistory::ShutDown()
{
	if (!mCommandDeq.empty())
		for (size_t i = 0; i < mCommandDeq.size(); ++i)
			delete mCommandDeq.at(i);
	mCommandDeq.clear();
}

Command* CommandHistory::GetCurrentCommand()
{
	if (!mCommandDeq.empty())
		return mCommandDeq.at(mCommandPointer);
	return nullptr;
}

CommandHistory::CommandHistory()
	: mCommandDeq{}
	, mCommandPointer(0) // Pointer value becomes 0 when the 1st element is inserted
	, mCMDStartPointer(0)
	, mCMDEndPointer(0)
	, mIsRecording(false)
	, mLatestActorCommand(nullptr)
{
}

CommandHistory::~CommandHistory()
{
}