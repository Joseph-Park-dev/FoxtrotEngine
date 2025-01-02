// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

Texture2D g_texture0 : register(t0); 
SamplerState g_sampler : register(s0);  

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET {
    
    return float4(input.color, 1.5);
}
