#pragma once
#include "ResourceSystem/FTMaterials/FTMaterial.h"

using Vector3 = DirectX::SimpleMath::Vector3;
struct RimData;

class RimMaterial :
	public FTMaterial
{
public:
	virtual void CreatePixelConstBuffer(ComPtr<ID3D11Device>& device, ComPtr<ID3D11Buffer>& buffer) override;
	virtual void UpdateBuffer(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11Buffer>& buffer) override;

	virtual void LoadFromFile() override;

public:
	RimMaterial();
	~RimMaterial() override;

private:
	RimData* mData;

#ifdef FOXTROT_EDITOR
public:
	virtual void SaveToFile() override;

	void UpdateUI() override;

#endif
};

struct RimData
{
	Vector3	 mRimColor = Vector3(1.0f);
	float	 mRimPower = 1.0f;
	float	 mRimStrength = 1.0f;
	uint32_t mUseSmoothstep = true;

	float dummy1;
	float dummy2;
};

namespace ChunkKey
{
	constexpr const char* RIM_MAT = "RimMaterial";
	namespace Rim
	{
		constexpr const char* COLOR = "Rim Color";
		constexpr const char* POWER = "Rim Power";
		constexpr const char* STRENGTH = "Rim Strength";
		constexpr const char* USE_SMOOTH_STEP = "Use Smooth Step";
	}
}