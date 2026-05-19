#include "ResourceSystem/Mesh/FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Common;
	Common::FTDS::DynamicArray<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

	FTMeshDataPack::FTMeshDataPack()
	{
	}
} // namespace D3D11