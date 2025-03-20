#include "FTCubemap.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

void FTCubemap::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	std::vector<FTMeshData> meshData;
	meshData.push_back(ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_BOX));
	FTBasicMeshGroup::Initialize(meshData, device, context);
}