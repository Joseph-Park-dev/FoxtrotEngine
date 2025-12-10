// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

struct VS_IN
{
    // Position in model coordinate.
    float3 posModel : POSITION;
};

struct GS_IN
{
    float4 posProj : SV_POSITION;
    float4 right : POSITION0;
};

struct PS_IN
{
    float4 posProj : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

float2 RotateUV90(float2 uv)
{
    // Shift to center
    uv -= 0.5;

    // Apply 90¡Æ rotation matrix
    // [ 0 -1 ]
    // [ 1  0 ]
    uv = float2(-uv.y, uv.x);

    // Shift back
    uv += 0.5;

    return uv;
}
