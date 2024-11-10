#include "Components/ColliderComponent.h"

#include <assert.h>
#include <DirectXColors.h>

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Renderer/FoxtrotRenderer.h"

#include "Physics/Physics2D.h"
#include "Managers/KeyInputManager.h"
#include "FileSystem/ChunkLoader.h"
#include "Components/Rigidbody2DComponent.h"

void ColliderComponent::CreateShape(b2BodyId bodyID, b2ShapeDef* shapeDef, b2Polygon* polygon)
{
	b2CreatePolygonShape(bodyID, shapeDef, polygon);
}

FTVector2 ColliderComponent::GetOffsetPos() const
{
	return mOffsetPos;
}

FTVector2 ColliderComponent::GetFinalPosition() const
{
	return mFinalPosition;
}

FTVector2& ColliderComponent::GetOffsetPosRef()
{
	return mOffsetPos;
}

void ColliderComponent::SetOffsetPos(FTVector2 offsetPos)
{
	mOffsetPos = offsetPos;
}

void ColliderComponent::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void ColliderComponent::LateUpdate(float deltaTime)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffsetPos;
}

ColliderComponent::ColliderComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mPolygon()
	, mOffsetPos(FTVector2::Zero)
	, mFinalPosition(FTVector2::Zero)
{
}

ColliderComponent::ColliderComponent(const ColliderComponent& origin)
	: Component(nullptr, origin.GetDrawOrder(), origin.GetUpdateOrder())
	, mOffsetPos(origin.mOffsetPos)
{
}

ColliderComponent::~ColliderComponent()
{
}

//void ColliderComponent::CloneTo(Actor* actor)
//{
//	ColliderComponent* newColliderComp = new ColliderComponent(actor, GetDrawOrder(), GetUpdateOrder());
//	newColliderComp->mOffsetPos = this->mOffsetPos;
//}

void ColliderComponent::OnCollisionEnter(ColliderComponent* other)
{
	GetOwner()->OnCollisionEnter(other);
}

void ColliderComponent::OnCollisionStay(ColliderComponent* other)
{
	GetOwner()->OnCollisionStay(other);
}

void ColliderComponent::OnCollisionExit(ColliderComponent* other)
{
	GetOwner()->OnCollisionExit(other);
}

void ColliderComponent::OnRayEnter()
{
	GetOwner()->OnRayEnter();
}

void ColliderComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
}

void ColliderComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui_impl_win32.h>
	#include <imgui_impl_dx11.h>

	#include "CommandHistory.h"

void ColliderComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
}

void ColliderComponent::EditorUpdate(float deltaTime)
{
}

void ColliderComponent::EditorRender(FoxtrotRenderer* renderer)
{

}

void ColliderComponent::EditorUIUpdate()
{
	UpdateOffsetPos();
}

void ColliderComponent::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
{}

void ColliderComponent::UpdateOffsetPos()
{
	FTVector2 updatedVal = GetOffsetPos();
	CommandHistory::GetInstance()->UpdateVector2Value("Offset Position", updatedVal, FLOATMOD_SPEED);
	mOffsetPos = updatedVal;
}
#endif