#include "FTMeshData.h"

#include <Dynamic/DynamicArray.h>

bool FTMeshData::IsEmpty() const
{
	return Vertices.IsEmpty() || Indices.IsEmpty();
}

bool FTDebugMeshData::IsEmpty() const
{
	return Vertices.empty(); // || Indices.empty();
}
