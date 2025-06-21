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

#include "InputSystem/FTInputDevice.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Components/Component.h"

#include "Static/FTString.h"

void EditorElement::UpdateUI(bool isPremade)
{
	if (mIsFocused)
	{
		ImGui::BeginChild(GetName().C_Str());
		if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Basic Data"))
			{
				UpdateActorName();
				UpdateDrawOrder();
				UpdateActorGroup();
				UpdateActorState();
				GetTransform()->UpdateUI();
				
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Components"))
			{
				UpdateComponentsUI();
				ImGui::EndTabItem();
			}
			if(!isPremade)
				UpdateMakePrefabBtn();
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}
}

const bool EditorElement::GetIsFocused() const 
{
	return mIsFocused;
}

const size_t EditorElement::GetHierarchyLevel() const 
{
	return mHierarchyLevel;
}

const bool EditorElement::GetIsDisplayed() const
{
	return mIsDisplayed;
}

void EditorElement::SetIsFocused(bool isFocused)
{
	mIsFocused = isFocused;
}

void EditorElement::SetHierarchyLevel(size_t lv)
{
	mHierarchyLevel = lv;
}

void EditorElement::SetIsDisplayed(bool isDisplayed)
{
	mIsDisplayed = isDisplayed;
}

void EditorElement::Initialize(FTCore* coreInst)
{
	Actor::Initialize(coreInst);

	EditorElement* buf = this;
	size_t level = 0;
	while (buf)
	{
		if (!buf->GetParent()) // No parent Actors.
			break;
		buf = dynamic_cast<EditorElement*>(buf->GetParent());
		++level;
	}
	mHierarchyLevel = level;
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

EditorElement::EditorElement()
	: Actor ()
	, mIsFocused(false)
	, mHierarchyLevel(0)
	, mIsDisplayed(false)
{}

EditorElement::EditorElement(Actor* actor)
	: Actor(actor)
	, mIsFocused(false)
	, mHierarchyLevel(0)
	, mIsDisplayed(false)
{
}

EditorElement::EditorElement(FTPremade* premade)
	: Actor(premade)
	, mIsFocused(false)
	, mHierarchyLevel(0)
	, mIsDisplayed(false)
{
}

void EditorElement::UpdateActorName()
{
	CommandHistory::GetInstance()->UpdateStringValue("Actor Name", GetNameRef());
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
				int grpIdx = ++n;
				ActorGroupEditCommand* command = DBG_NEW ActorGroupEditCommand(GetActorGroupRef());
				command->SetNextVal(static_cast<ActorGroup>(grpIdx));
				CommandHistory::GetInstance()->AddCommand(command);

				SetActorGroup((ActorGroup)grpIdx);
			}
		}
		ImGui::EndCombo();
	}
}

void EditorElement::UpdateDrawOrder()
{
	int i = GetDrawOrder();
	ImGui::InputInt("Draw Order", &i);
	SetDrawOrder(i);
}

void EditorElement::UpdateActorState()
{
	CommandHistory::GetInstance()->UpdateStateValue("Actor State", GetStateRef());
}

void EditorElement::UpdateComponentsUI()
{
	if (ImGui::BeginChild(GetName().C_Str()))
	{
		size_t count = 0;
		for (Component* comp : GetComponents())
		{
			FTDS::String name(std::to_string(count).c_str());
			name.Append(" ");
			name.Append(comp->GetName());

			if (ImGui::TreeNode(name.C_Str()))
			{
				int updateOrder = comp->GetUpdateOrder();
				ImGui::InputInt(ChunkKey::UPDATE_ORDER, &updateOrder);
				comp->SetUpdateOrder(updateOrder);

				comp->EditorUIUpdate();
				if (ImGui::SmallButton("Delete")) 
					RemoveComponent(comp);
				ImGui::TreePop();
			}
			++count;
		}
		if (FTEditorUtils::ButtonCenteredOnLine("Add Component"))
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
			if (ImGui::Selectable((*iter).first))
				(*iter).second(this, FTCoreEditor::GetInstance());
		ImGui::EndPopup();
	}
}

void EditorElement::UpdateMakePrefabBtn()
{
	if (FTEditorUtils::ButtonCenteredOnLine("Make Premade"))
	{
		bool confirmed = false;
		EditorLayer::GetInstance()->SetInfoType(InfoType::PremadeIsCreated);
	}
}