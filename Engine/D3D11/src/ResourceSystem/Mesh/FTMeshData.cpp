#include "ResourceSystem/Mesh/FTMeshData.h"

#include "FTDS/Dynamic/DynamicArray.h"

namespace D3D11
{
	/// @brief Tests whether the container has no logical elements.
	/// @return True when the container has no logical elements; otherwise false.
	bool FTMeshData::IsEmpty() const
	{
		return Vertices.IsEmpty() || Indices.IsEmpty();
	}

	/// @brief Tests whether the container has no logical elements.
	/// @return True when the container has no logical elements; otherwise false.
	bool FTDebugMeshData::IsEmpty() const
	{
		return Vertices.IsEmpty(); // || Indices.empty();
	}
} // namespace D3D11
