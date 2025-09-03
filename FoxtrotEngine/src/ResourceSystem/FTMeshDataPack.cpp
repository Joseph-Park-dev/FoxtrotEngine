#include "FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"
#include "Components/Component.h"

std::vector<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

FTMeshDataPack::FTMeshDataPack()
{
}