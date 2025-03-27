#include "FTCubemap.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

void FTCubemap::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, float scale)
{
	std::vector<FTMeshData> meshData;
	FTMeshData box = ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_BOX, scale);
	meshData.push_back(box);

	FTBasicMeshGroup::Initialize(meshData, device, context);
}