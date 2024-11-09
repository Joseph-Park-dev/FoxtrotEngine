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

UINT ColliderComponent::g_nextID = 0;

void ColliderComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
}

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

FTVector2 ColliderComponent::GetScale() const
{
	return mScale;
}

CollidedSide ColliderComponent::GetCollidedSide() const
{
	return mCollidedSide;
}

uint32_t ColliderComponent::GetID() const
{
	return mID;
}

FTVector2& ColliderComponent::GetOffsetPosRef()
{
	return mOffsetPos;
}

FTVector2& ColliderComponent::GetScaleRef()
{
	return mScale;
}

void ColliderComponent::SetOffsetPos(FTVector2 offsetPos)
{
	mOffsetPos = offsetPos;
}

void ColliderComponent::SetScale(FTVector2 scale)
{
	mScale = scale;
}

void ColliderComponent::Initialize(FTCore* coreInstance)
{
	mPolygon = b2MakeBox(mScale.x / 2, mScale.y / 2);
	Component::Initialize(coreInstance);
}

void ColliderComponent::LateUpdate(float deltaTime)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPosition();
	mFinalPosition = ownerPos + mOffsetPos;
	assert(0 <= mColliCount);
}

ColliderComponent::ColliderComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mOffsetPos(FTVector2::Zero)
	, mFinalPosition(FTVector2::Zero)
	, mScale(FTVector2(50, 50))
	, mColliCount(0)
	, mCollidedSide(CollidedSide::NONE)
	, mID(g_nextID++)
{
}

ColliderComponent::ColliderComponent(const ColliderComponent& origin)
	: Component(nullptr, origin.GetDrawOrder(), origin.GetUpdateOrder())
	, mOffsetPos(origin.mOffsetPos)
	, mScale(origin.mScale)
	, mID(g_nextID++)
	, mCollidedSide(CollidedSide::NONE)
	, mColliCount(0)
{
}

ColliderComponent::~ColliderComponent()
{
}

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
	UpdateScale();
}

void ColliderComponent::RenderDebugGeometries(FoxtrotRenderer* renderer, ImDrawList* imDrawList, FTVector2 screenCenter)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPositionYInverted();
	FTVector2 min = screenCenter + ownerPos + mOffsetPos - (mScale / 2);
	FTVector2 max = screenCenter + ownerPos + mOffsetPos + (mScale / 2);
	imDrawList->AddRect(
		min.GetImVec2(), 
		max.GetImVec2(),
		ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 0.f, 0.f, 255.f)),
		0.0f,
		0,
		3.0f
		);
}

void ColliderComponent::UpdateOffsetPos()
{
	FTVector2 updatedVal = GetOffsetPos();
	CommandHistory::GetInstance()->UpdateVector2Value("Offset Position", updatedVal, FLOATMOD_SPEED);
	mOffsetPos = updatedVal;
}

void ColliderComponent::UpdateScale()
{
	FTVector2 updatedVal = GetScale();
	CommandHistory::GetInstance()->UpdateVector2Value("Scale", updatedVal, FLOATMOD_SPEED);
	mScale = updatedVal;
}
#endif