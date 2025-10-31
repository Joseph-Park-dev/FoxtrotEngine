#include "FTRectangle.h"

#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"
#include "Managers/DebugShapes.h"

using Matrix = DirectX::SimpleMath::Matrix;

void FTRectangle::Initialize(FoxtrotRenderer* renderer)
{
	FTShape::Initialize(renderer);
	InitializeMesh(renderer->GetDevice(), GeometryGenerator::MakeDebugPoint(FTVector4(1.f, 0.f, 0.f, 1.f)));
}

FTRectangle::FTRectangle()
	: FTShape()
{
}

FTRectangle::~FTRectangle()
{
}

void FTRectangle::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRECTANGLE);
	FileIOHelper::SaveBool(ofs, ChunkKey::FTSHAPE_IS_ACTIVE, GetIsActive());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRECTANGLE);
}

void FTRectangle::LoadProperties(std::ifstream& ifs)
{
	bool isActive = false;

	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTRECTANGLE);
	FileIOHelper::LoadBool(ifs, isActive);

	SetIsActive(isActive);
}

#ifdef FOXTROT_EDITOR
void FTRectangle::UpdateUI()
{
	bool isActive = GetIsActive();
	CommandHistory::GetInstance()->UpdateBoolValue("Render Shape", isActive);
	SetIsActive(isActive);
}
#endif