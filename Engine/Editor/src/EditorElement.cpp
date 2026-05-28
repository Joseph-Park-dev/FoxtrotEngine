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
#include "EditorSceneManager.h"
#include "ChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "EditorUtils.h"
#include "Actor/EditorTransform.h"

#include "InputSystem/IInputDevice.h"
#include "Manager/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "Actor/Transform.h"
#include "Actor/ActorGroup.h"
#include "Actor/IActor.h"
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
					CommandHistory::GetInstance()->UpdateBoolValue("Is Active", IsActive());
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

	// void EditorElement::EditorUpdate(float deltaTime)
	//{
	//	for (auto comp = GetComponents()->Begin(); comp != GetComponents()->End(); ++comp)
	//	{
	//		if (*comp)
	//		{
	//			if ((*comp)->GetIsActive())
	//				(*comp)->EditorUpdate(deltaTime);
	//		}
	//	}
	// }

	// void EditorElement::EditorRender(D3D11Renderer* renderer)
	//{
	//	for (auto comp = GetComponents().Begin(); comp != GetComponents().End(); ++comp)
	//	{
	//		if (*comp)
	//		{
	//			if ((*comp)->GetIsActive())
	//				(*comp)->EditorRender(renderer);
	//		}
	//	}
	// }

	EditorElement::EditorElement(int id)
		: Actor(id)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Actor* actor, int id)
		: Actor(actor, id)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Core::Actor* actor, int id, bool deepCpyChild)
		: Core::Actor(actor, id, deepCpyChild)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		SwitchTransformToEditor();
	}

	EditorElement::EditorElement(Core::FTPremade* premade, int id)
		: Core::Actor(premade, id)
		, mIsFocused(false)
		, mHierarchyLevel(0)
		, mIsDisplayed(false)
	{
		SwitchTransformToEditor();
	}

	void EditorElement::AddChild(IActor* actor)
	{
	}

	void EditorElement::RemoveChild(IActor* actor)
	{
	}

	void EditorElement::RemoveComponent(IComponent* component)
	{
	}

	void EditorElement::RemoveAllComponents()
	{
	}

	void EditorElement::CopyTransformFrom(IActor* actor)
	{
	}

	void EditorElement::CopyComponentsFrom(IActor* actor)
	{
	}

	void EditorElement::CopyChildObjectFrom(IActor* actor)
	{
		if (GetChildActors()->GetSize() < 1)
			return;

		actor->GetChildActors()->IterateArray([&](Actor* child) {
			if (child)
			{
				Editor::ChunkLoader::GetInstance()->AddMaxActorID();
				int maxID = Editor::ChunkLoader::GetInstance()->GetMaxActorID();
				this->AddChild(DBG_NEW Actor(child, maxID));
			}
		});
	}

	void EditorElement::RefChildObjectFrom(IActor* actor)
	{
	}

	ActorData* EditorElement::GetData()
	{
		return nullptr;
	}

	ActorGroup EditorElement::GetActorGroup() const
	{
		return ActorGroup();
	}

	ActorGroup& EditorElement::GetActorGroupRef()
	{
		// TODO: insert return statement here
	}

	ActorGroup* EditorElement::GetActorGroupPtr()
	{
		return nullptr;
	}

	Common::FTDS::String EditorElement::GetName()
	{
		return Common::FTDS::String();
	}

	Common::FTDS::String& EditorElement::GetNameRef()
	{
		// TODO: insert return statement here
	}

	const int EditorElement::GetID() const
	{
		return 0;
	}

	const bool& EditorElement::GetIsActive() const
	{
		// TODO: insert return statement here
	}

	Transform* EditorElement::GetTransform() const
	{
		return nullptr;
	}

	IActor* EditorElement::GetParent() const
	{
		return nullptr;
	}

	Common::FTDS::DynamicArray<IComponent*>* EditorElement::GetComponents()
	{
		return nullptr;
	}

	Common::FTDS::DynamicArray<IActor*>* EditorElement::GetChildActors()
	{
		return nullptr;
	}

	const int& EditorElement::GetDrawOrder() const
	{
		// TODO: insert return statement here
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
			for (auto comp = GetComponents()->Begin(); comp != GetComponents()->End(); ++comp)
			{
				if (GetComponents()->IsEmpty())
					break;
				if (*comp)
				{
					Common::FTDS::String name(std::to_string(count).c_str());
					name.Append(" ");
					name.Append((*comp)->GetName());

					if (ImGui::TreeNode(name.C_Str()))
					{
						CommandHistory::GetInstance()->UpdateIntValue(Core::ChunkKey::UPDATE_ORDER, (*comp)->UpdateOrder());
						(*comp)->EditorUIUpdate();
						if (ImGui::SmallButton("Delete"))
							RemoveComponent((*comp));
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

	EditorElement* CreateEditorElementFromActor(Core::Actor* actor, int id)
	{
		return DBG_NEW EditorElement(actor, id);
	}
	void EditorElement::SetName(Common::FTDS::String&& name)
	{
	}
	void EditorElement::SetIsActive(bool isActive)
	{
	}
	void EditorElement::SetActorGroup(ActorGroup group)
	{
	}
	void EditorElement::SetState(ActorState state)
	{
	}
	void EditorElement::SetParent(IActor* parent)
	{
	}
	void EditorElement::SetTransform(Transform* transform)
	{
	}
	void EditorElement::SetComponents(Common::FTDS::DynamicArray<IComponent*>* components)
	{
	}
	void EditorElement::SetChildActors(Common::FTDS::DynamicArray<IActor*>* children)
	{
	}
	void EditorElement::SetDrawOrder(int order)
	{
	}
	bool EditorElement::HasName(Common::FTDS::String&& name)
	{
		return false;
	}
	bool EditorElement::HasName(const char* name)
	{
		return false;
	}
	bool EditorElement::IsDead()
	{
		return false;
	}
	bool& EditorElement::IsActive()
	{
		// TODO: insert return statement here
	}
	void EditorElement::SaveProperties(std::ofstream& ofs)
	{
	}
	void EditorElement::SaveComponents(std::ofstream& ofs)
	{
	}
	void EditorElement::LoadProperties(std::ifstream& ifs)
	{
	}
	void EditorElement::LoadComponents(std::ifstream& ifs)
	{
	}
} // namespace Editor