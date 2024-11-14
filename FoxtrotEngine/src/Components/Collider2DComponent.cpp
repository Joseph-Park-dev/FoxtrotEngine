#include "Components/Collider2DComponent.h"

#include <assert.h>
#include <DirectXColors.h>

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"

#include "Physics/Physics2D.h"
#include "Managers/KeyInputManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Components/Rigidbody2DComponent.h"

b2ShapeId& Collider2DComponent::GetShapeID()
{
	return mShapeID;
}

FTVector2 Collider2DComponent::GetOffsetPos() const
{
	return mOffset;
}

FTVector2 Collider2DComponent::GetFinalPosition() const
{
	return mFinalPosition;
}

FTVector2& Collider2DComponent::GetOffsetPosRef()
{
	return mOffset;
}

void Collider2DComponent::SetOffsetPos(FTVector2 offsetPos)
{
	mOffset = offsetPos;
}

void Collider2DComponent::Initialize(FTCore* coreInstance)
{
	LogString("Warning : Non-overrided Collider2D::Initialize() is being used -> Not affected by Simulation");
}

void Collider2DComponent::LateUpdate(float deltaTime)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffset;
}

Collider2DComponent::Collider2DComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mOffset(FTVector2::Zero)
	, mFinalPosition(FTVector2::Zero)
{
}

Collider2DComponent::Collider2DComponent(const Collider2DComponent& origin)
	: Component(nullptr, origin.GetDrawOrder(), origin.GetUpdateOrder())
	, mOffset(origin.mOffset)
{
}

Collider2DComponent::~Collider2DComponent()
{
}

//void Collider2DComponent::CloneTo(Actor* actor)
//{
//	Collider2DComponent* newColliderComp = new Collider2DComponent(actor, GetDrawOrder(), GetUpdateOrder());
//	newColliderComp->mOffsetPos = this->mOffsetPos;
//}

void Collider2DComponent::OnCollisionEnter(Collider2DComponent* other)
{
	GetOwner()->OnCollisionEnter(other);
}

void Collider2DComponent::OnCollisionStay(Collider2DComponent* other)
{
	GetOwner()->OnCollisionStay(other);
}

void Collider2DComponent::OnCollisionExit(Collider2DComponent* other)
{
	GetOwner()->OnCollisionExit(other);
}

void Collider2DComponent::OnRayEnter()
{
	GetOwner()->OnRayEnter();
}

void Collider2DComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::OFFSET, mOffset);
}

void Collider2DComponent::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mOffset);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui_impl_win32.h>
	#include <imgui_impl_dx11.h>

	#include "CommandHistory.h"

void Collider2DComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
}

void Collider2DComponent::EditorUpdate(float deltaTime)
{
}

void Collider2DComponent::EditorRender(FoxtrotRenderer* renderer)
{

}

void Collider2DComponent::EditorUIUpdate()
{
	UpdateOffsetPos();
}

void Collider2DComponent::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
{}

void Collider2DComponent::UpdateOffsetPos()
{
	FTVector2 updatedVal = GetOffsetPos();
	CommandHistory::GetInstance()->UpdateVector2Value("Offset Position", updatedVal, FLOATMOD_SPEED);
	mOffset = updatedVal;
}
#endif