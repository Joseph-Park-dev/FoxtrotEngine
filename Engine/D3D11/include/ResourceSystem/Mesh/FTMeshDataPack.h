#pragma once
#include "FTMeshData.h"

namespace D3D11
{
	class FTMeshDataPack
	{
	public:
		/// @brief Returns the mesh data used by this ftmesh data pack.
		/// @return Borrowed access to the mesh data.
		/// @note Changes through the returned reference affect this object's stored state.
		Common::FTDS::DynamicArray<FTMeshData*>& GetMeshData();

	public:
		/// @brief Initializes storage for a group of mesh-data resources.
		FTMeshDataPack();

	private:
		Common::FTDS::DynamicArray<FTMeshData*> mMeshData;
	};

	namespace ChunkKey
	{
		constexpr const char* FT_MESH = "FTMesh";
	} // namespace ChunkKey
} // namespace D3D11
