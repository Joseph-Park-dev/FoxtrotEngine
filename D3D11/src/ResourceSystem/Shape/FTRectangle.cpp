#include "FTRectangle.h"

#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"
#include "Manager/DebugShapes.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"

namespace D3D11
{
	void FTRectangle::Initialize(D3D11Renderer* renderer)
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
} // namespace D3D11