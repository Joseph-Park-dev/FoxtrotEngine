// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

// SV_Target -> 결과가 render target 에 저장
float4 main(PixelShaderInput input) : SV_TARGET
{
    return float4(input.color, 0.5);
}
