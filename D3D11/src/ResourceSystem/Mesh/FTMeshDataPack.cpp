#include "FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"
#include "Component/Component.h"

namespace D3D11
{
	std::vector<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

	FTMeshDataPack::FTMeshDataPack()
	{
	}
} // namespace D3D11