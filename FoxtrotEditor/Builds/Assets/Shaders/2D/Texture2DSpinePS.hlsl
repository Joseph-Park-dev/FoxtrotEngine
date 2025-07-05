// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "../Common.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PixelConstantBuffer : register(b0)
{
    float3 eyeWorld;
    bool useTexture;
    Light lights[MAX_LIGHTS];
    BlinnPhongData material;
};

float4 main(TexPSInput input) : SV_Target
{
    float3 color = float3(0.0, 0.0, 0.0);
    float alphaThres = 0.05;
    
    float4 result = useTexture ? float4(color, 1.0) * g_texture0.Sample(g_sampler, input.texcoord) : float4(color, 1.0);
    if (result.w < alphaThres)
        clip(-1); // Discards the pixel
    
    return result;
}