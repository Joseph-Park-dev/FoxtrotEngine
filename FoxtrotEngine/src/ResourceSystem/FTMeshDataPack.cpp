#include "FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"
#include "Components/Component.h"

std::vector<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

FTMeshDataPack::FTMeshDataPack()
	: FTResource()
{
}

void FTMeshDataPack::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_MESH);
	FTResource::SaveProperties(ofs, key);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_MESH);
}

UINT FTMeshDataPack::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_MESH);
	return FTResource::LoadProperties(ifs);
}
