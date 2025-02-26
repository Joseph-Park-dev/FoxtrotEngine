#include "FTMeshData.h"

std::vector<Vertex>&   FTMeshData::GetVertices() { return mVertices; }
std::vector<uint32_t>& FTMeshData::GetIndices() { return mIndices; }

const size_t FTMeshData::VerticesCount() const { return mVertices.size(); }
const size_t FTMeshData::IndicesCount() const { return mIndices.size(); }

bool FTMeshData::IsEmpty() const
{
	return mVertices.empty() || mIndices.empty();
}

FTMeshData::FTMeshData()
	: FTResource()
{
	std::vector<Vertex>	  vertices;
	std::vector<uint32_t> indices;
}
