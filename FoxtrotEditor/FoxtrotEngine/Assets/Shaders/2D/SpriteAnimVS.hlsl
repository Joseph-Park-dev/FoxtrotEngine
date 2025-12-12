// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteAnim.hlsli"

cbuffer VSConst : register(b0)
{
    matrix modelMat;
};

GS_IN main(VS_IN vsInput)
{
    GS_IN output;
    
    // Calculating world space position
    float4 pos = float4(vsInput.posModel, 1.0);
    pos = mul(pos, modelMat);
    output.posWorld = pos;
    
    // Calculating right direction 
    float4 right = float4(modelMat[0][0], modelMat[1][0], modelMat[2][0], 0.0);
    output.right = normalize(right);
   
    return output;
}