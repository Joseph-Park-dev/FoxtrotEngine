#include "FTMeshData.h"

bool FTMeshData::IsEmpty() const
{
	return Vertices.empty() || Indices.empty();
}
