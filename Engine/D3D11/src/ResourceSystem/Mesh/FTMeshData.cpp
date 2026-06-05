#include "ResourceSystem/Mesh/FTMeshData.h"

#include "FTDS/Dynamic/DynamicArray.h"

namespace Graphics
{
	bool FTMeshData::IsEmpty() const
	{
		return Vertices.IsEmpty() || Indices.IsEmpty();
	}

	bool FTDebugMeshData::IsEmpty() const
	{
		return Vertices.IsEmpty(); // || Indices.empty();
	}
} // namespace Graphics