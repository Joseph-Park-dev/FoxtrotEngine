// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Common.hlsli"
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

SolidPSInput main(SolidVSInput input)
{
    SolidPSInput output;
    
    float4 pos = float4(input.posModel, 1.0);
    pos = mul(pos, model);
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose);
    output.normalWorld = normalize(output.normalWorld);
    
    output.color = input.color;
    
    return output;
}