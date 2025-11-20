// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Common.hlsli"

cbuffer CBVertex : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

TexPSInput main(SpineVSInput vsInput)
{
    TexPSInput output;
    
    // Calculating world space position
    float4 pos = float4(vsInput.posModel, 0.0, 1.0);
    pos = mul(pos, model);
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view); // Camera space position
    pos = mul(pos, projection); // Clip space position
    output.posProj = pos;
    
    // Assigning texture coordinates
    output.texcoord = vsInput.texcoord;
   
    return output;
}