#include "Components/BoxCollider2DComponent.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
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
	GetPolygonRef() = b2MakeBox(mScale.x / 2, mScale.y / 2);
	Collider2DComponent::Initialize(coreInstance);
}

BoxCollider2DComponent::BoxCollider2DComponent(Actor* owner, int drawOrder, int updateOrder)
	: Collider2DComponent(owner, drawOrder, updateOrder)
	, mScale(FTVector2(50.f,50.f))
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
	mScale = updatedVal;
}
#endif // FOXTROT_EDITOR