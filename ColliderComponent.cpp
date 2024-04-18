 #include <assert.h>

#include "ColliderComponent.h"
#include "Actor.h"
#include "Transform.h"
#include "Camera2D.h"
#include "EditorCamera2D.h"
#include "Transform.h"
#include "Physics2D.h"
#include "KeyInputManager.h"
#include "ChunkLoader.h"
#include "CommandHistory.h"
#include "Transform.h"

#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#endif // DEBUG

UINT ColliderComponent::g_nextID = 0;

void ColliderComponent::LateUpdate(float deltaTime)
{
	Vector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffsetPos;
	assert(0 <= mColliCount);
}

#ifdef _DEBUG
void ColliderComponent::Render(FoxtrotRenderer* renderer)
{
	/*
	*	Alternative for
			SDL_SetRenderDrawColor
			(
				renderer,
				0, 255, 0, 255
			);
			Vector2 renderPos =
				EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(mFinalPosition);
			SDL_Rect rect =
			{
				static_cast<int>(renderPos.x - mScale.x * 0.5f * (1 - EditorCamera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(renderPos.y - mScale.y * 0.5f * (1 - EditorCamera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(mScale.x * (1 - EditorCamera2D::GetInstance()->GetZoomValue())),
				static_cast<int>(mScale.y * (1 - EditorCamera2D::GetInstance()->GetZoomValue()))
			};
			SDL_RenderDrawRect(renderer, &rect);
	*/
}
#endif // _DEBUG

ColliderComponent::ColliderComponent(Actor* owner, int drawOrder, int updateOrder)
	:Component(owner, drawOrder, updateOrder)
	, mOffsetPos(Vector2::Zero)
	, mFinalPosition(Vector2::Zero)
	, mScale(Vector2::Zero)
	, mColliCount(0)
	, mCollidedSide(CollidedSide::NONE)
	, mID(g_nextID++)
{}

ColliderComponent::ColliderComponent(const ColliderComponent & origin)
	:Component(nullptr, GetDrawOrder())
	, mOffsetPos(origin.mOffsetPos)
	, mScale(origin.mScale)
	, mID(g_nextID++)
	, mCollidedSide(CollidedSide::NONE)
	, mColliCount(0)
{

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
	Vector2 updatedVal = GetOffsetPos();
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
		updatedVal = Vector2(vec2[0], vec2[1]);
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
	Vector2 updatedVal = GetScale();
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
		updatedVal = Vector2(vec2[0], vec2[1]);
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
