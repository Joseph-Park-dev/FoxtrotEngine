#include "ResourceSystem/Mesh/FTMeshDataPack.h"

#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Common;
	/// @brief Returns the mesh data used by this ftmesh data pack.
	/// @return Borrowed access to the mesh data.
	/// @note Changes through the returned reference affect this object's stored state.
	Common::FTDS::DynamicArray<FTMeshData*>& FTMeshDataPack::GetMeshData() { return mMeshData; }

	/// @brief Initializes storage for a group of mesh-data resources.
	/// @note Initializes the :FTMeshDataPack base or delegates to its constructor.
	FTMeshDataPack::FTMeshDataPack()
	{
	}
} // namespace D3D11
