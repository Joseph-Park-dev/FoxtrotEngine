#pragma once
#include "ResourceSystem/FTMaterials/FTMaterial.h"

#include <wrl.h>

struct Light;
struct StandardMatData;
class FTBasicMeshGroup;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

class StandardMaterial :
	public FTMaterial
{
public:
	// void AssignData(StandardMatData& standardDest, BlinnPhongData& blinnPhongDest);
	virtual void CreatePixelConstBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11Buffer>& buffer) override;
	virtual void UpdateBuffer(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11Buffer>& buffer) override;

	virtual void SaveToFile() override;
	virtual void LoadFromFile() override;

public:
	StandardMaterial();
	~StandardMaterial() override;

private:
	StandardMatData* mData;

public:
	virtual void SaveProperties(std::ofstream& ofs);
	virtual void LoadProperties(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;

#endif
};

struct BlinnPhongData
{
	FTVector3 Ambient	= FTVector3(0.5f);
	float	  Shininess = 0.5f;
	FTVector3 Diffuse	= FTVector3(0.5f);
	float	  dummy1;
	FTVector3 Specular = FTVector3(0.5f);
	float	  dummy2;
};

struct StandardMatData
{
	Vector3		   EyeWorld;
	uint32_t	   UseTexture;
	Light		   Lights[GameData::MAX_LIGHTS];
	BlinnPhongData BlinnPhongData;
};

namespace ChunkKey
{
	constexpr const char* STANDARD_MAT = "StandardMaterial";
	constexpr const char* USE_TEXTURE = "Use Texture";

	namespace BlinnPhong
	{
		constexpr const char* AMBIENT	= "Ambient";
		constexpr const char* SHININESS = "Shininess";
		constexpr const char* DIFFUSE	= "Diffuse";
		constexpr const char* SPECULAR	= "Specular";
	}
}

static_assert((sizeof(StandardMatData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");