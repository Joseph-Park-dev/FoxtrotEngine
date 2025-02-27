#pragma once
#include "ResourceSystem/FTResource.h"

#include <vector>

#include "FTMeshData.h"

class FTMeshDataPack :
    public FTResource
{
public:
    std::vector<FTMeshData>& GetMeshData();

public:
    FTMeshDataPack();

private:
    std::vector<FTMeshData> mMeshData;

public:
    virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
    virtual UINT LoadProperties(std::ifstream& ifs) override;
};

namespace ChunkKey
{
    constexpr const char* FT_MESH = "FTMesh";
} // namespace ChunkKey