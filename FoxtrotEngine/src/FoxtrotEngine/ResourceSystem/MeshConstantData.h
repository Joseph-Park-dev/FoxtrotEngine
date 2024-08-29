#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"

using Microsoft::WRL::ComPtr;

struct BasicVertexConstantData {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix invTranspose;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(BasicVertexConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct BasicPixelConstantData {
    DirectX::SimpleMath::Vector3 eyeWorld;         // 12
    float mipmapLevel = 0.0f; // 4
    //DirectX::SimpleMath::Vector3 eyeWorld;         // 12
    //bool useTexture;          // bool 1 + 3 padding
    //Material material;        // 48
    //Light lights[MAX_LIGHTS]; // 48 * MAX_LIGHTS
    //Vector4 indexColor; // 피킹(Picking)에 사용
};
static_assert((sizeof(BasicPixelConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");