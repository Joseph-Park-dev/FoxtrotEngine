#pragma once
#include <vector>

#include "FTMeshData.h"

class FTMeshDataPack
{
public:
    std::vector<FTMeshData*>& GetMeshData();

public:
    FTMeshDataPack();

private:
    std::vector<FTMeshData*> mMeshData;
};

namespace ChunkKey
{
    constexpr const char* FT_MESH = "FTMesh";
} // namespace ChunkKey