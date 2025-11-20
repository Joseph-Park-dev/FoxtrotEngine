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
    float alphaTrim;
    float3 dummy;
    BlinnPhongData material;
};

float4 main(TexPSInput input) : SV_Target
{
    float4 result = useTexture ? g_texture0.Sample(g_sampler, input.texcoord) : float4(0.f, 0.f, 0.f, 1.f);
    
    if(result.w < alphaTrim)
        clip(-1);
    
    return result;
}