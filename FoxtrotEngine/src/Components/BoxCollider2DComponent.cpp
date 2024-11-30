#include "Components/BoxCollider2DComponent.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Components/Rigidbody2DComponent.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
// Used to access Debug geometries
#include "ResourceSystem/GeometryGenerator.h"
#include "DebugGeometries.h"
#endif

const FTVector2 BoxCollider2DComponent::GetScale() const
{
    return mScale;
}

void BoxCollider2DComponent::SetScale(FTVector2 scale)
{
	mScale = scale;
}

void BoxCollider2DComponent::Initialize(FTCore* coreInstance)
{
	Rigidbody2DComponent* rb = GetOwner()->GetComponent<Rigidbody2DComponent>();
	if (rb)
	{
		if (b2Body_IsValid(rb->GetBodyID()))
		{
			b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
			b2Polygon polygon = b2MakeBox(mScale.x / 2, mScale.y / 2);
			GetShapeID() = b2CreatePolygonShape(rb->GetBodyID(), &polygonShapeDef, &polygon);
		}
		else
			LogString("ERROR : BoxCollider2DComponent::Initialize() -> BodyId not valid");
	}
	Component::Initialize(coreInstance);

#ifdef FOXTROT_EDITOR
	mDebugRect = DBG_NEW FTRectangle;
	mDebugRect->Initialize(coreInstance->GetGameRenderer());
#endif // FOXTROT_EDITOR
}

BoxCollider2DComponent::BoxCollider2DComponent(Actor* owner, int drawOrder, int updateOrder)
	: Collider2DComponent(owner, drawOrder, updateOrder)
	, mScale(FTVector2(10.f, 10.f))
#ifdef FOXTROT_EDITOR
	, mDebugRect(nullptr)
#endif
{
}

BoxCollider2DComponent::~BoxCollider2DComponent()
{
	if (b2Shape_IsValid(GetShapeID()))
		b2DestroyShape(GetShapeID(), true);
#ifdef FOXTROT_EDITOR
	DebugGeometries::GetInstance()->RemoveShape(mDebugRect);
	mDebugRect = nullptr;
#endif
}

void BoxCollider2DComponent::SaveProperties(std::ofstream& ofs)
{
	Collider2DComponent::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::COLLIDER_SCALE, mScale);
}

void BoxCollider2DComponent::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mScale);
	Collider2DComponent::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void BoxCollider2DComponent::EditorUpdate(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();
	FTVector3 pos = transform->GetWorldPosition();
	pos = pos * FTVector3(1.f, -1.f, 1.f);
	FTVector3 rot = transform->GetRotation();
	FTVector3 scale = FTVector3(
		transform->GetScale().x * mScale.x,
		transform->GetScale().y * mScale.y,
		1.f
	);

	UpdateDebugGeometries(pos, rot, scale, Camera::GetInstance());
}

void BoxCollider2DComponent::EditorRender(FoxtrotRenderer* renderer)
{}

void BoxCollider2DComponent::EditorUIUpdate()
{
	Collider2DComponent::EditorUIUpdate();
	UpdateScale();
}

void BoxCollider2DComponent::UpdateDebugGeometries(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* cameraInst)
{
	/*float NDC = Camera::GetInstance()->GetNDCRatio();
	pos *= NDC;
	scale *= NDC;*/

	mDebugRect->Update(
		pos,
		rot,
		scale,
		Camera::GetInstance()
	);
}

void BoxCollider2DComponent::UpdateScale()
{
	FTVector2 updatedVal = GetScale();
	CommandHistory::GetInstance()->UpdateVector2Value("Scale", updatedVal, FLOATMOD_SPEED);
	if (mScale != updatedVal)
	{
		mScale = updatedVal;
		if (mScale.x <= 0 || mScale.y <= 0)
			return;
		b2ShapeDef polygonShapeDef = b2DefaultShapeDef();
		b2Polygon polygon = b2MakeBox(mScale.x / 2, mScale.y / 2);
		if (b2Shape_IsValid(GetShapeID()))
		{
			//b2DestroyShape(GetShapeID(), true);
			b2Shape_SetPolygon(GetShapeID(), &polygon);
		}
	}
}

void BoxCollider2DComponent::CloneTo(Actor* actor)
{
	BoxCollider2DComponent* newComp = DBG_NEW BoxCollider2DComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->SetOffsetPos(this->GetOffsetPos());
	newComp->mScale = this->mScale;
}
#endif // FOXTROT_EDITOR