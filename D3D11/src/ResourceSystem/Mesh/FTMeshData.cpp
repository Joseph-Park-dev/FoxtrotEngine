#include "FTMeshData.h"

#include "FTDS/Dynamic/DynamicArray.h"

namespace D3D11
{
	bool FTMeshData::IsEmpty() const
	{
		return Vertices.IsEmpty() || Indices.IsEmpty();
	}

	bool FTDebugMeshData::IsEmpty() const
	{
		return Vertices.empty(); // || Indices.empty();
	}
} // namespace D3D11