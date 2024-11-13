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
#include "FileSystem/ChunkLoader.h"
#include "Actors/Transform.h"
#include "Actors/ActorGroup.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Components/Component.h"

void EditorElement::UpdateUI()
{
	if (GetIsFocused())
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
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}
	CheckMouseHover();
}

void EditorElement::RenderUI(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void EditorElement::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
{
	for (auto comp : GetComponents()) 
	{
		comp->RenderDebugGeometries(imDrawList, screenCenter);
	}
}

void EditorElement::OnMouseLButtonClicked()
{
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	scene->UnfocusEditorElements();
	SetIsFocused(true);
	std::vector<Actor*>* actors = scene->GetActors();
	for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
	{
		for (int j = 0; j < actors[i].size(); ++j)
		{
			EditorElement* elems = dynamic_cast<EditorElement*>(actors[i][j]);
			if (elems->GetIsFocused())
			{
				int& mActorNameIDX = EditorLayer::GetInstance()->GetActorNameIdx();
				CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIDX, i));
				break;
			}
		}
	}
}

void EditorElement::CheckMouseHover()
{
	FTVector2 mousePos = KeyInputManager::GetInstance()->GetMousePosition();
	if (GetIsAffectedByCamera())
	{
		//mousePos = EditorCamera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
	}
	if (GetTransform()->GetWorldPosition().x - GetTransform()->GetScale().x / 2 <= mousePos.x &&
		mousePos.x <= GetTransform()->GetWorldPosition().x + GetTransform()->GetScale().x / 2 &&
		GetTransform()->GetWorldPosition().y - GetTransform()->GetScale().y / 2 <= mousePos.y &&
		mousePos.y <= GetTransform()->GetWorldPosition().y + GetTransform()->GetScale().y / 2)
	{
		SetMouseHovering(true);
	}
	else
	{
		SetMouseHovering(false);
	}
}

void EditorElement::OnMouseHovering()
{
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow))
		PanelUI::OnMouseHovering();
}

void EditorElement::OnMouseLButtonDown()
{
	if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow))
		PanelUI::OnMouseLButtonDown();
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

EditorElement::EditorElement(Scene* scene)
	: PanelUI (scene,false)
	, mRotationModSpeed(1.0f)
	, mActorGroupIdx((int)ActorGroup::DEFAULT)
{}

EditorElement::EditorElement(Actor* actor, Scene* scene)
	: PanelUI(actor, scene)
	, mActorGroupIdx((int)actor->GetActorGroup())
	, mRotationModSpeed(1.0f)
{
	SetActorGroup(actor->GetActorGroup());
	SetName(actor->GetName());
	SetState(actor->GetState());
	SetParent(actor->GetParent());
	SetChildActors(actor->GetChildActors());
}

void EditorElement::UpdateActorName()
{
	char* updatedName = _strdup(GetName().c_str());
	if (ImGui::InputText("Name", updatedName, ACTORNAME_MAX))
	{
		if (EditorLayer::GetInstance()->GetConfirmKeyPressed())
		{
			CommandHistory::GetInstance()->
				AddCommand(new StrEditCommand(GetNameRef(), updatedName));
		}
	}
}

void EditorElement::UpdateActorGroup()
{
	const char* comboPreview = ActorGroupUtil::GetActorGroupStr(mActorGroupIdx);
	if (ImGui::BeginCombo("Actor Group", comboPreview))
	{
		for (size_t n = 0; n < (size_t)ActorGroup::END - 1; n++)
		{
			if (ImGui::Selectable(ActorGroupUtil::GetActorGroupStr(n)))
			{
				CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorGroupIdx, n));
				mActorGroupIdx = n;
				SetActorGroup((ActorGroup)mActorGroupIdx);
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
		CommandHistory::GetInstance()->AddCommand(new StateEditCommand(GetStateRef(), state));
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