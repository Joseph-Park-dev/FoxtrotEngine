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

DebugGSIN main(DebugVSInput input)
{
    DebugGSIN output;
    
    float4 pos = float4(input.posModel, 1.0);
    
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);
   
    output.pos = pos;
    output.color = float4(input.color, 1.0);
    
    return output;
}