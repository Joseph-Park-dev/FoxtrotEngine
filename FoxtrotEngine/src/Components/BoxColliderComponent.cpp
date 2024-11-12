#include "Components/BoxColliderComponent.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#endif

const FTVector2 BoxColliderComponent::GetScale() const
{
    return mScale;
}

void BoxColliderComponent::SetScale(FTVector2 scale)
{
	mScale = scale;
}

void BoxColliderComponent::Initialize(FTCore* coreInstance)
{
	ColliderComponent::Initialize(coreInstance);
	GetPolygonRef() = b2MakeBox(mScale.x / 2, mScale.y / 2);
}

BoxColliderComponent::BoxColliderComponent(Actor* owner, int drawOrder, int updateOrder)
	: ColliderComponent(owner, drawOrder, updateOrder)
	, mScale(FTVector2(50.f,50.f))
{
}

void BoxColliderComponent::SaveProperties(std::ofstream& ofs)
{
	ColliderComponent::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKeys::COLLIDER_SCALE, mScale);
}

void BoxColliderComponent::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mScale);
	ColliderComponent::LoadProperties(ifs);
}

void BoxColliderComponent::EditorUpdate(float deltaTime)
{
}

void BoxColliderComponent::EditorRender(FoxtrotRenderer* renderer)
{
}

void BoxColliderComponent::EditorUIUpdate()
{
	ColliderComponent::EditorUIUpdate();
	UpdateScale();
}

void BoxColliderComponent::RenderDebugGeometries(ImDrawList* imDrawList, FTVector2 screenCenter)
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
void BoxColliderComponent::UpdateScale()
{
	FTVector2 updatedVal = GetScale();
	CommandHistory::GetInstance()->UpdateVector2Value("Scale", updatedVal, FLOATMOD_SPEED);
	mScale = updatedVal;
}
#endif // FOXTROT_EDITOR