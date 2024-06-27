#include "FoxtrotEngine/Components/ColliderComponent.h"

#include <assert.h>

#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Renderer/Camera2D.h"
#include "FoxtrotEngine/Physics/Physics2D.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

UINT ColliderComponent::g_nextID = 0;

void ColliderComponent::LateUpdate(float deltaTime)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffsetPos;
	assert(0 <= mColliCount);
}

ColliderComponent::ColliderComponent(Actor* owner, int drawOrder, int updateOrder)
	:Component(owner, drawOrder, updateOrder)
	, mOffsetPos(FTVector2::Zero)
	, mFinalPosition(FTVector2::Zero)
	, mScale(FTVector2(50,50))
	, mColliCount(0)
	, mCollidedSide(CollidedSide::NONE)
	, mID(g_nextID++)
{}

ColliderComponent::ColliderComponent(const ColliderComponent & origin)
	: Component(nullptr, origin.GetDrawOrder(), origin.GetUpdateOrder())
	, mOffsetPos(origin.mOffsetPos)
	, mScale(origin.mScale)
	, mID(g_nextID++)
	, mCollidedSide(CollidedSide::NONE)
	, mColliCount(0)
{}

void ColliderComponent::CloneTo(Actor* actor)
{
	ColliderComponent* newColliderComp = new ColliderComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newColliderComp->mOffsetPos = this->mOffsetPos;
	newColliderComp->mScale = this->mScale;
	newColliderComp->mID = ++g_nextID;
}

void ColliderComponent::OnCollisionEnter(ColliderComponent* other)
{
	GetOwner()->OnCollisionEnter(other);
	++mColliCount;
}

void ColliderComponent::OnCollisionStay(ColliderComponent* other)
{
	GetOwner()->OnCollisionStay(other);
}

void ColliderComponent::OnCollisionExit(ColliderComponent* other)
{
	GetOwner()->OnCollisionExit(other);
	--mColliCount;
}

void ColliderComponent::OnRayEnter()
{
	GetOwner()->OnRayEnter();
}

#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#include "FoxtrotEditor/EditorCamera2D.h"
#include "FoxtrotEditor/CommandHistory.h"

void ColliderComponent::Render(FoxtrotRenderer* renderer)
{
	FTVector2 topLeft = mFinalPosition - mScale / 2;
	FTVector2 bottomRight = mFinalPosition + mScale / 2;

	FTVector2 topLeftScreenPos =
		EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(topLeft);
	FTVector2 bottomRightScreenPos =
		EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(bottomRight);
	renderer->DrawRectangle(topLeftScreenPos, bottomRightScreenPos);
}

void ColliderComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::AddVector2(ofs, mOffsetPos);
	FileIOHelper::AddVector2(ofs, mScale);
}

void ColliderComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	FileIOHelper::LoadVector2(ifs, mOffsetPos);
	FileIOHelper::LoadVector2(ifs, mScale);
}

void ColliderComponent::EditorLateUpdate(float deltaTime)
{
	LateUpdate(deltaTime);
}

void ColliderComponent::EditorUIUpdate()
{
	UpdateOffsetPos();
	UpdateScale();
}

void ColliderComponent::UpdateOffsetPos()
{
	FTVector2 updatedVal = GetOffsetPos();
	float* vec2 = new float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat2("Offset Position", vec2))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(GetOffsetPosRef(), GetOffsetPos()));
		}
		updatedVal = FTVector2(vec2[0], vec2[1]);
		SetOffsetPos(updatedVal);
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(GetOffsetPosRef(), updatedVal));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete[] vec2;
}

void ColliderComponent::UpdateScale()
{
	FTVector2 updatedVal = GetScale();
	float* vec2 = new float[2];
	vec2[0] = updatedVal.x;
	vec2[1] = updatedVal.y;
	bool isRecording = CommandHistory::GetInstance()->GetIsRecording();
	if (ImGui::DragFloat2("Scale", vec2))
	{
		if (!isRecording && ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->SetIsRecording(true);
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(GetScaleRef(), GetScale()));
		}
		updatedVal = FTVector2(vec2[0], vec2[1]);
		SetScale(updatedVal);
	}
	else
	{
		if (isRecording && !ImGui::IsMouseDown(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			CommandHistory::GetInstance()->
				AddCommand(new Vector2EditCommand(GetScaleRef(), updatedVal));
			CommandHistory::GetInstance()->SetIsRecording(false);
		}
	}
	delete[] vec2;
}
#endif // DEBUG