// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorElement.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorChunkLoader.h"
#include "CommandHistory.h"
#include "EditorUtils.h"

#include "Managers/KeyInputManager.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Components/Component.h"

void EditorElement::UpdateUI(bool isPremade)
{
	if (mIsFocused)
	{
		ImGui::BeginChild(GetName().c_str());
		if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Basic Data"))
			{
				UpdateActorName();
				UpdateActorGroup();
				UpdateActorState();

				UpdateActorWorldPosition();
				UpdateActorLocalPosition();
				UpdateActorScale();
				UpdateActorRotation();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Components"))
			{
				UpdateComponents();
				ImGui::EndTabItem();
			}
			if(!isPremade)
				UpdateMakePrefabBtn();
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}
}

void EditorElement::EditorUpdate(float deltaTime)
{
	for (auto comp : GetComponents()) 
		comp->EditorUpdate(deltaTime);
}

void EditorElement::EditorRender(FoxtrotRenderer* renderer)
{
	for (auto comp : GetComponents())
		comp->EditorRender(renderer);
}

EditorElement::EditorElement(EditorScene* scene)
	: Actor (scene)
	, mIsFocused(false)
{}

EditorElement::EditorElement(Actor* actor)
	: Actor(actor)
	, mIsFocused(false)
{}

EditorElement::EditorElement(Actor* actor, EditorScene* scene)
	: Actor(actor, scene)
	, mIsFocused(false)
{}

void EditorElement::UpdateActorName()
{
	char* updatedName = _strdup(GetName().c_str());
	if (ImGui::InputText("Name", updatedName, ACTORNAME_MAX))
	{
		if (EditorLayer::GetInstance()->GetConfirmKeyPressed())
		{
			CommandHistory::GetInstance()->
				AddCommand(DBG_NEW StrEditCommand(GetNameRef(), updatedName));
		}
	}
	delete updatedName;
}

void EditorElement::UpdateActorGroup()
{
	const char* comboPreview = ActorGroupUtil::GetActorGroupStr(GetActorGroup());
	if (ImGui::BeginCombo("Actor Group", comboPreview))
	{
		for (size_t n = 0; n <= ActorGroupUtil::GetCount()-1; ++n)
		{
			if (ImGui::Selectable(ActorGroupUtil::GetActorGroupStr(n)))
			{
				int grpIdx = static_cast<int>(GetActorGroup());
				CommandHistory::GetInstance()->AddCommand(DBG_NEW IntEditCommand(grpIdx, ++n));
				grpIdx = n;
				SetActorGroup((ActorGroup)grpIdx);
			}
		}
		ImGui::EndCombo();
	}
}

void EditorElement::UpdateActorState()
{
	State state = GetState();
	bool isActive = IsActive();
	ImGui::Checkbox("Active", &isActive);
	if (isActive)
		state = State::EActive;
	else
		state = State::EDead;

	if (state != GetState())
		CommandHistory::GetInstance()->AddCommand(DBG_NEW StateEditCommand(GetStateRef(), state));
}

void EditorElement::UpdateActorWorldPosition()
{
	FTVector3 updatedVal = GetTransform()->GetWorldPosition();
	CommandHistory::GetInstance()->UpdateVector3Value
	(
		"World Position",
		updatedVal,
		FLOATMOD_SPEED
	);
	GetTransform()->SetWorldPosition(updatedVal);
}

void EditorElement::UpdateActorLocalPosition()
{
	FTVector3 updatedVal = GetTransform()->GetLocalPosition();
	CommandHistory::GetInstance()->UpdateVector3Value
	(
		"Local Position",
		updatedVal,
		FLOATMOD_SPEED
	);
	GetTransform()->SetLocalPosition(updatedVal);
}

void EditorElement::UpdateActorScale()
{
	FTVector3 updatedVal = GetTransform()->GetScale();
	CommandHistory::GetInstance()->UpdateVector3Value
	(
		"Scale",
		updatedVal,
		FLOATMOD_SPEED
	);
	GetTransform()->SetScale(updatedVal);
}

void EditorElement::UpdateActorRotation()
{
	FTVector3 degreeRot = Transform::ConvertRadToDegree(GetTransform()->GetRotation());
	CommandHistory::GetInstance()->UpdateVector3Value
	(
		"Rotation",
		degreeRot,
		FLOATMOD_SPEED
	);
	GetTransform()->SetRotation(Transform::ConvertDegreeToRad(degreeRot));
}

void EditorElement::UpdateComponents()
{
	if (ImGui::BeginChild(GetName().c_str()))
	{
		size_t count = 0;
		for (Component* comp : GetComponents())
		{
			std::string name = std::to_string(count) + " " + comp->GetName();
			if (ImGui::TreeNode(name.c_str()))
			{
				comp->EditorUIUpdate();
				if (ImGui::SmallButton("Delete")) 
					RemoveComponent(comp);
				ImGui::TreePop();
			}
			++count;
		}
		if (ButtonCenteredOnLine("Add Component"))
			ImGui::OpenPopup("CompSelectPopUp");
		DisplayCompSelectionPopup();

		ImGui::EndChild();
	}
}

void EditorElement::DisplayCompSelectionPopup()
{
	if (ImGui::BeginPopup("CompSelectPopUp"))
	{
		ImGui::SeparatorText("Add Components");
		ComponentCreateMap::iterator iter = EditorChunkLoader::GetInstance()->GetCompCreateMap().begin();
		for (; iter != EditorChunkLoader::GetInstance()->GetCompCreateMap().end(); ++iter)
			if (ImGui::Selectable((*iter).first.c_str()))
				(*iter).second(this, FTCoreEditor::GetInstance());
		ImGui::EndPopup();
	}
}

void EditorElement::UpdateMakePrefabBtn()
{
	if (ButtonCenteredOnLine("Make Premade"))
	{
		bool confirmed = false;
		EditorLayer::GetInstance()->SetInfoType(InfoType::PremadeIsCreated);
	}
}