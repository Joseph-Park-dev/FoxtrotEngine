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

void EditorElement::RenderUI(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
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
	: Actor()
	, mActorGroupIdx(0)
	, mRotationModSpeed(0.f)
	, mIsFocused(false)
{
}

EditorElement::EditorElement(Actor* origin)
	: Actor(origin)
	, mActorGroupIdx(0)
	, mRotationModSpeed(1.0f)
	, mIsFocused(false)
{
	SetActorGroup(origin->GetActorGroup());
	SetName(origin->GetName());
	SetState(origin->GetState());
	SetParent(origin->GetParent());
	SetChildActors(origin->GetChildActors());

	CopyTransformFrom(origin);
	CopyComponentsFrom(origin);
	CopyChildObject(origin);
}

EditorElement::EditorElement(Scene* scene)
	: Actor (scene)
	, mRotationModSpeed(1.0f)
	, mActorGroupIdx((int)ActorGroup::DEFAULT)
	, mIsFocused(false)
{}

EditorElement::EditorElement(Actor* actor, Scene* scene)
	: Actor(actor, scene)
	, mActorGroupIdx((int)actor->GetActorGroup())
	, mRotationModSpeed(1.0f)
	, mIsFocused(false)
{
	SetActorGroup(actor->GetActorGroup());
	SetName(actor->GetName());
	SetState(actor->GetState());
	SetParent(actor->GetParent());
	SetChildActors(actor->GetChildActors());
}

EditorElement::EditorElement(EditorElement* origin, Scene* scene)
	: Actor(origin, scene)
	, mActorGroupIdx((int)origin->GetActorGroup())
	, mRotationModSpeed(1.0f)
	, mIsFocused(false)
{
	SetActorGroup(origin->GetActorGroup());
	SetState(origin->GetState());
	SetParent(origin->GetParent());
	SetChildActors(origin->GetChildActors());
	//CopyComponents(origin);
}

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
				CommandHistory::GetInstance()->AddCommand(DBG_NEW IntEditCommand(mActorGroupIdx, n));
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