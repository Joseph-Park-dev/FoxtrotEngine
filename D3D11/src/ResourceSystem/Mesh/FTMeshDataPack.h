#pragma once
#include "FTMeshData.h"

namespace D3D11
{
	class FTMeshDataPack
	{
	public:
		FTDS::DynamicArray<FTMeshData*>& GetMeshData();

	public:
		FTMeshDataPack();

	private:
		FTDS::DynamicArray<FTMeshData*> mMeshData;
	};

	namespace ChunkKey
	{
		constexpr const char* FT_MESH = "FTMesh";
	} // namespace ChunkKey
} // namespace D3D11