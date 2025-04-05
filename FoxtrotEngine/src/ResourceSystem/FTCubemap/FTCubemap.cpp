#include "FTCubemap.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

void FTCubemap::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, float scale)
{
	//FTBasicMeshGroup* box = ResourceManager::GetInstance()->GetLoadedMesh(ChunkKey::PRIMITIVE_BOX);
	//FTBasicMeshGroup::Initialize({ box }, device, context);
}