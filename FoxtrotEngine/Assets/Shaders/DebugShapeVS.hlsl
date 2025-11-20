// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Common.hlsli"
cbuffer DBGShapeVSConst : register(b0)
{
    matrix model;
};

DebugGSIN main(DebugVSInput input)
{
    DebugGSIN output;
    
    float4 pos = float4(input.posModel, 1.0);
    pos = mul(pos, model);
    output.pos = pos;
    
    float4 right = float4(model[0][0], model[1][0], model[2][0], 0.0);
    output.right = normalize(right);
    output.right = float4(-output.right.x, output.right.y, output.right.z, output.right.w);
    output.color = float4(input.color, 1.0);
    
    return output;
}