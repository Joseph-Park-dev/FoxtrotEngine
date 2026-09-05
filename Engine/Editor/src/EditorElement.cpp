// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorElement.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>

#include "EditorLayer.h"
#include "ChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "Component/IEditorComponent.h"
#include "EditorUtils.h"
#include "Actor/EditorTransform.h"

#include "InputSystem/IInputDevice.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "Actor/Transform.h"
#include "Actor/ActorGroup.h"
#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "Component/IComponent.h"
#include "ResourceSystem/FTPremade.h"

#include "FTDS/Static/FTString.h"

namespace Editor
{
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
					CommandHistory::GetInstance()->UpdateBoolValue("Is Active", GetIsActiveRef());
					UpdateTransformUI();

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Components"))
				{
					UpdateComponentsUI();
					ImGui::EndTabItem();
				}
				if (!isPremade)
					UpdateMakePremade();

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

	void EditorElement::Initialize()
	{
		EditorElement* buf	 = this;
		size_t		   level = 0;
		while (buf)
		{
			if (!buf->GetParent()) // No parent Actors.
				break;
			buf = dynamic_cast<EditorElement*>(buf->GetParent());
			++level;
		}
		mHierarchyLevel = level;
	}

	EditorElement::EditorElement(int id)
		: mData(DBG_NEW Common::ActorData)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		mData->Name		  = "New Empty Actor";
		mData->ID		  = id;
		mData->ActorGroup = Core::ActorGroup::DEFAULT;
		mData->State	  = Core::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Core::Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Core::IComponent*>;
		mData->Parent	  = nullptr;
		mData->Children	  = DBG_NEW Common::FTDS::DynamicArray<Core::IActor*>;
		mData->DrawOrder  = 0;

		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Core::IActor* actor, int id)
		: mData(DBG_NEW Common::ActorData)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		mData->Name = "New Copied Actor";
		mData->ID	= id;
		SetActorGroup(actor->GetActorGroup());
		mData->State	  = Core::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Core::Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Core::IComponent*>;
		SetParent(actor->GetParent());
		mData->Children	 = DBG_NEW Common::FTDS::DynamicArray<Core::IActor*>;
		mData->DrawOrder = actor->GetData()->DrawOrder;

		mData->Name.Assign(actor->GetNameRef());

		CopyChildObjectFrom(actor);
		CopyTransformFrom(actor);
		CopyComponentsFrom(actor);

		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Core::IActor* actor, int id, bool deepCpyChild)
		: mData(DBG_NEW Common::ActorData)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		mData->Name = "New Copied Actor";
		mData->ID	= id;
		SetActorGroup(actor->GetActorGroup());
		mData->State	  = Core::ActorState::ALIVE;
		mData->IsActive	  = true;
		mData->Transform  = DBG_NEW	 Core::Transform(this);
		mData->Components = DBG_NEW Common::FTDS::DynamicArray<Core::IComponent*>;
		SetParent(actor->GetParent());
		mData->Children	 = DBG_NEW Common::FTDS::DynamicArray<Core::IActor*>;
		mData->DrawOrder = actor->GetData()->DrawOrder;

		mData->Name.Assign(actor->GetNameRef());

		if (deepCpyChild)
			CopyChildObjectFrom(actor);
		else
			RefChildObjectFrom(actor);

		CopyTransformFrom(actor);
		CopyComponentsFrom(actor);

		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Core::FTPremade* premade, int id)
		: EditorElement(reinterpret_cast<Core::IActor*>(premade->GetOrigin()), id)
	{
		SwitchTransformToEditor();
	}

	void EditorElement::AddChild(IActor* child)
	{
		mData->AddChild(child);
	}

	void EditorElement::RemoveChild(IActor* child)
	{
		mData->RemoveChild(child);
	}

	void EditorElement::RemoveComponent(Core::IComponent* component)
	{
		mData->RemoveComponent(component);
	}

	void EditorElement::RemoveAllComponents()
	{
		mData->RemoveAllComponents();
	}

	void EditorElement::CopyTransformFrom(IActor* actor)
	{
		mData->CopyTransformFrom(actor);
	}

	void EditorElement::CopyComponentsFrom(IActor* actor)
	{
		mData->CopyComponentsFrom(actor);
	}

	void EditorElement::CopyChildObjectFrom(IActor* actor)
	{
		mData->CopyChildObjectFrom<EditorElement>(actor);
	}

	void EditorElement::RefChildObjectFrom(IActor* actor)
	{
		mData->RefChildObjectFrom(actor);
	}

	Common::ActorData* EditorElement::GetData()
	{
		return mData;
	}

	Core::ActorGroup EditorElement::GetActorGroup() const
	{
		return mData->ActorGroup;
	}

	Core::ActorGroup& EditorElement::GetActorGroupRef()
	{
		return mData->ActorGroup;
	}

	Core::ActorGroup* EditorElement::GetActorGroupPtr()
	{
		return &mData->ActorGroup;
	}

	Common::FTDS::String EditorElement::GetName()
	{
		return mData->Name;
	}

	Common::FTDS::String& EditorElement::GetNameRef()
	{
		return mData->Name;
	}

	const int EditorElement::GetID() const
	{
		return mData->ID;
	}

	const bool& EditorElement::GetIsActive() const
	{
		return mData->IsActive;
	}

	bool& EditorElement::GetIsActiveRef()
	{
		return mData->IsActive;
	}

	Core::Transform* EditorElement::GetTransform() const
	{
		return mData->Transform;
	}

	Editor::EditorElement* EditorElement::GetParent() const
	{
		return reinterpret_cast<EditorElement*>(mData->Parent);
	}

	Common::FTDS::DynamicArray<Core::IComponent*>* EditorElement::GetComponents()
	{
		return mData->Components;
	}

	Common::FTDS::DynamicArray<Core::IActor*>* EditorElement::GetChildActors()
	{
		return mData->Children;
	}

	const int& EditorElement::GetDrawOrder() const
	{
		return mData->DrawOrder;
	}

	void EditorElement::SetName(Common::FTDS::String&& name)
	{
		mData->Name.Assign(name);
	}

	void EditorElement::SetIsActive(bool isActive)
	{
		mData->IsActive = isActive;
	}

	void EditorElement::SetActorGroup(Core::ActorGroup group)
	{
		mData->ActorGroup = group;
	}

	void EditorElement::SetState(Core::ActorState state)
	{
		mData->State = state;
	}

	void EditorElement::SetParent(IActor* parent)
	{
		mData->Parent = parent;
		parent->AddChild(this);
	}

	void EditorElement::SetTransform(Core::Transform* transform)
	{
		mData->Transform = transform;
	}

	void EditorElement::SetComponents(Common::FTDS::DynamicArray<Core::IComponent*>* components)
	{
		mData->Components = components;
	}

	void EditorElement::SetChildActors(Common::FTDS::DynamicArray<IActor*>* children)
	{
		mData->Children = children;
	}

	void EditorElement::SetDrawOrder(int order)
	{
		mData->DrawOrder = order;
	}

	bool EditorElement::HasName(Common::FTDS::String&& name)
	{
		return mData->Name.Equal(name.C_Str());
	}

	bool EditorElement::HasName(const char* name)
	{
		return Common::FTDS::StringEqual(mData->Name.C_Str(), name);
	}

	bool EditorElement::IsDead()
	{
		return mData->State == Core::ActorState::DEAD;
	}

	void EditorElement::UpdateActorName()
	{
		CommandHistory::GetInstance()->UpdateStringValue("Actor Name", GetNameRef());
	}

	void EditorElement::UpdateActorGroup()
	{
		const char* comboPreview = Core::ActorGroupUtil::GetActorGroupStr(GetActorGroup());
		if (ImGui::BeginCombo("Actor Group", comboPreview))
		{
			for (size_t n = 0; n <= Core::ActorGroupUtil::GetCount() - 1; ++n)
			{
				if (ImGui::Selectable(Core::ActorGroupUtil::GetActorGroupStr(n)))
				{
					int							   grpIdx  = static_cast<int>(++n);
					Editor::ActorGroupEditCommand* command = DBG_NEW Editor::ActorGroupEditCommand(GetActorGroupRef());
					command->SetNextVal(static_cast<Core::ActorGroup>(grpIdx));
					// CommandHistory::GetInstance()->AddCommand(command);

					SetActorGroup((Core::ActorGroup)grpIdx);
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

	void EditorElement::UpdateTransformUI()
	{
		Core::Transform* transform = GetTransform();
	}

	void EditorElement::UpdateComponentsUI()
	{
		if (ImGui::BeginChild(GetName().C_Str()))
		{
			size_t count = 0;
			for (auto iter = GetComponents()->Begin(); iter != GetComponents()->End(); ++iter)
			{
				if (GetComponents()->IsEmpty())
					break;
				if (*iter)
				{
					Common::FTDS::String name(std::to_string(count).c_str());
					name.Append(" ");
					name.Append((*iter)->GetName());

					if (ImGui::TreeNode(name.C_Str()))
					{
						//CommandHistory::GetInstance()->UpdateIntValue(Core::ChunkKey::UPDATE_ORDER, (*comp)->UpdateOrder());
						(*iter)->EditorUIUpdate(CommandHistory::GetInstance());
						if (ImGui::SmallButton("Delete"))
							RemoveComponent(*iter);
						ImGui::TreePop();
					}
					++count;
				}
			}
			if (Editor::ButtonCenteredOnLine("Add Component"))
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
			// ComponentCreateMap::iterator iter = EditorChunkLoader::GetInstance()->GetCompCreateMap().begin();
			// for (; iter != EditorChunkLoader::GetInstance()->GetCompCreateMap().end(); ++iter)
			//	if (ImGui::Selectable((*iter).first))
			//		(*iter).second(this);
			ImGui::EndPopup();
		}
	}

	void EditorElement::UpdateMakePremade()
	{
		if (Editor::ButtonCenteredOnLine("Make Premade"))
		{
			bool confirmed = false;
			EditorLayer::GetInstance()->SetInfoType(InfoType::PremadeIsCreated);
		}
	}
	void EditorElement::SwitchTransformToEditor()
	{
		if (GetTransform())
			delete GetTransform();
		SetTransform(DBG_NEW EditorTransform(this));
	}

	EditorElement* CreateEditorElementFromActor(Core::IActor* actor, int id)
	{
		return DBG_NEW EditorElement(actor, id);
	}
} // namespace Editor