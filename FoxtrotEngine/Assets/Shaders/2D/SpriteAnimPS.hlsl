// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteAnim.hlsli"

Texture2D g_texture0 : register(t0);
SamplerState g_sampler : register(s0);

cbuffer PSCBuf : register(b0)
{
    float4 color;
    bool useTexture;
    bool3 dummy;
};

float4 main(PS_IN input) : SV_Target
{
    float alphaThres = 0.05;
    
    float4 result = useTexture ? g_texture0.Sample(g_sampler, input.texCoord) * color : color;
    if (result.w < alphaThres)
        clip(-1); // Discards the pixel
    
    return result;
}