#pragma once
#include "ResourceSystem/FTBasicMeshGroup.h"

class FTCubemap :
	public FTBasicMeshGroup
{
public:
	void Initialize(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);
};
