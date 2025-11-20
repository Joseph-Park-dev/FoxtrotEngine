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
    float4 color;
};

float4 main(TexPSInput input) : SV_Target
{
    //float3 toEye = normalize(eyeWorld - input.posWorld);

    //float3 color = float3(0.0, 0.0, 0.0);
    
    //int i = 0;
    
    float alphaThres = 0.05;
    
    //[unroll] // warning X3557: loop only executes for 1 iteration(s), forcing loop to unroll
    //for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    //{
    //    color += ComputeDirectionalLight(lights[i], material, input.normalWorld, toEye);
    //}
    
    //[unroll]
    //for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    //{
    //    color += ComputePointLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    //}
    
    //[unroll]
    //for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    //{
    //    color += ComputeSpotLight(lights[i], material, input.posWorld, input.normalWorld, toEye);
    //}

    float4 result = useTexture ? g_texture0.Sample(g_sampler, input.texcoord): color;
    if (result.w < alphaThres)
        clip(-1); // Discards the pixel
    
    return result;
}