#include "Components/BoxCollider2DComponent.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "Components/Rigidbody2DComponent.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "DebugGeometries.h"

// Used to access primitive geometries
#include "ResourceSystem/GeometryGenerator.h"
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
	DebugGeometries::GetInstance()->AddPolygons(GeometryGenerator::MakeSquare(0.1f), &mScale);
#endif // FOXTROT_EDITOR
}

BoxCollider2DComponent::BoxCollider2DComponent(Actor* owner, int drawOrder, int updateOrder)
	: Collider2DComponent(owner, drawOrder, updateOrder)
	, mScale(FTVector2(10.f,10.f))
{
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

void BoxCollider2DComponent::EditorUpdate(float deltaTime)
{
}

void BoxCollider2DComponent::EditorRender(FoxtrotRenderer* renderer)
{
}

void BoxCollider2DComponent::EditorUIUpdate()
{
	Collider2DComponent::EditorUIUpdate();
	UpdateScale();
}

void BoxCollider2DComponent::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
{
	FTVector2 ownerPos = GetOwner()->GetTransform()->GetWorldPositionYInverted();
	FTVector2 min = screenCenter + ownerPos + GetOffsetPos() - (mScale / 2);
	FTVector2 max = screenCenter + ownerPos + GetOffsetPos() + (mScale / 2);
	imDrawList->AddRect(
		min.GetImVec2(),
		max.GetImVec2(),
		ImGui::ColorConvertFloat4ToU32(ImVec4(255.f, 0.f, 0.f, 255.f)),
		0.0f,
		0,
		3.0f
	);
}

#ifdef FOXTROT_EDITOR
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
#endif // FOXTROT_EDITOR