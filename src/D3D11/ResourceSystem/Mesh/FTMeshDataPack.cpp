#include "FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"
#include "Component/Component.h"

namespace D3D11
{
	Core::FTDS::DynamicArray<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

	FTMeshDataPack::FTMeshDataPack()
	{
	}
} // namespace D3D11