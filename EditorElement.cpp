#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "EditorElement.h"
#include "EditorLayer.h"
#include "Transform.h"
#include "ActorGroup.h"
#include "EditorCamera2D.h"
#include "KeyInputManager.h"
#include "CommandHistory.h"
#include "ChunkLoader.h"

#define ACTORNAME_MAX 100

void EditorElement::EditorLateUpdateActor()
{
	CheckMouseHover();
}

void EditorElement::UIUpdate()
{
	if (GetIsFocused())
	{
		if (ImGui::Begin(GetNameStr().c_str()))
		{
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
					UpdateActorRotation(&Transform::SetRotation);
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Components"))
				{
					UpdateComponents();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::End();
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			std::vector<EditorElement*>& editElems = EditorLayer::GetInstance()->GetEditorElements();
			std::vector<EditorElement*>::iterator iter;
			iter = std::find(editElems.begin(), editElems.end(), this);
			editElems.erase(iter);
		}
	}
}

void EditorElement::OnMouseLButtonClicked()
{
	std::vector<EditorElement*>& elems = EditorLayer::GetInstance()->GetEditorElements();
	for (int i = 0; i < elems.size(); ++i)
	{
		if (elems[i]->GetIsFocused())
			elems[i]->SetIsFocused(false);
	}
	SetIsFocused(true);
	for (int i = 0; i < elems.size(); ++i)
	{
		if (elems[i]->GetIsFocused())
		{
			int& mActorNameIDX = EditorLayer::GetInstance()->GetActorNameIdx();
			CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIDX, i));
			break;
		}
	}
}

void EditorElement::CheckMouseHover()
{
	Vector2 mousePos = KeyInputManager::GetInstance()->GetMousePosition();
	if (GetIsAffectedByCamera())
	{
		mousePos = EditorCamera2D::GetInstance()->ConvertScreenPosToWorld(mousePos);
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

void EditorElement::EditorUpdateComponents(float deltaTime)
{
	if (IsActive())
	{
		for (auto comp : GetComponents())
		{
			comp->EditorUpdate(deltaTime);
		}
	}
}

void EditorElement::EditorLateUpdateComponents(float deltaTime)
{
	if (IsActive())
	{
		for (auto comp : GetComponents())
		{
			comp->EditorLateUpdate(deltaTime);
		}
	}
}

void EditorElement::EditorUpdateActor()
{
}

void EditorElement::Render(FoxtrotRenderer* renderer)
{
	if (IsActive())
		RenderComponents(renderer);
}

void EditorElement::UIRender(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

EditorElement::EditorElement(Scene* scene)
	: PanelUI (scene,false)
{}

EditorElement::EditorElement(Actor* actor)
	: PanelUI(actor)
	, mActorGroupIdx((int)actor->GetActorGroup())
{
	SetActorGroup(actor->GetActorGroup());
	SetName(actor->GetName());
	SetState(actor->GetState());
	SetTransform(actor->GetTransform());
	SetComponents(actor->GetComponents());
	SetParent(actor->GetParent());
	SetChildActors(actor->GetChildActors());
}

EditorElement::~EditorElement()
{
}

void EditorElement::UpdateActorName()
{
	char* updatedName = _strdup(GetNameStr().c_str());
	if (ImGui::InputText("Name", updatedName, ACTORNAME_MAX))
	{
		if (EditorLayer::GetInstance()->GetConfirmKeyPressed())
		{
			CommandHistory::GetInstance()->
				AddCommand(new StrEditCommand(GetNameRef(), ToWString(updatedName)));
		}
	}
}

void EditorElement::UpdateActorGroup()
{
	const char* comboPreview = ActorGroup[mActorGroupIdx];
	if (ImGui::BeginCombo("Actor Group", comboPreview))
	{
		for (size_t n = 0; n < (size_t)ACTOR_GROUP::END - 1; n++)
		{
			if (ImGui::Selectable(ActorGroup[n]))
			{
				CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorGroupIdx, n));
				mActorGroupIdx = n;
				SetActorGroup((ACTOR_GROUP)mActorGroupIdx);
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
	CommandHistory::GetInstance()->UpdateVectorValue
	(
		"World Position",
		GetTransform()->GetWorldPositionRef(),
		GetTransform()->GetWorldPosition()
	);
}

void EditorElement::UpdateActorLocalPosition()
{
	CommandHistory::GetInstance()->UpdateVectorValue
	(
		"Local Position",
		GetTransform()->GetLocalPositionRef(),
		GetTransform()->GetLocalPosition()
	);
}

void EditorElement::UpdateActorScale()
{
	CommandHistory::GetInstance()->UpdateVectorValue
	(
		"Scale",
		GetTransform()->GetScaleRef(),
		GetTransform()->GetScale()
	);
}

void EditorElement::UpdateActorRotation(TransSetFloatFunc func)
{
	CommandHistory::GetInstance()->UpdateFloatValue
	(
		"Rotation",
		GetTransform()->GetRotationRef(),
		Math::ToDegrees(GetTransform()->GetRotation()),
		GetTransform(),
		func
	);
}

void EditorElement::UpdateComponents()
{
	if (ImGui::Begin(GetNameStr().c_str()))
	{
		size_t count = 0;
		for (Component* comp : GetComponents())
		{
			std::string name = std::to_string(count) + " " + ToString(comp->GetName());
			if (ImGui::TreeNode(name.c_str()))
			{
				comp->EditorUIUpdate();
				if (ImGui::SmallButton("Delete")) 
					RemoveComponent(comp);
				ImGui::TreePop();
			}
			++count;
		}
		ImGui::End();
	}

	if (ImGui::Button("AddComponent"))
	{
		ImGui::OpenPopup("CompSelectPopUp");
	}
	DisplayCompSelectionPopup();
}

void EditorElement::DisplayCompSelectionPopup()
{
	if (ImGui::BeginPopup("CompSelectPopUp"))
	{
		ImGui::SeparatorText("Add Components");
		ComponentCreateMap::iterator iter = ChunkLoader::GetInstance()->GetCompCreateMap().begin();
		for (; iter != ChunkLoader::GetInstance()->GetCompCreateMap().end(); ++iter)
			if (ImGui::Selectable(ToString((*iter).first).c_str()))
				(*iter).second(this);

		ImGui::EndPopup();
	}
}
