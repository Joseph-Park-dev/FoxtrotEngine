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
    float3 posModel : position0;
};

struct GS_IN
{
    float4 posWorld : SV_POSITION;
};

struct PS_IN
{
    float4 posProj : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

float2 RotateTexCoord(float2 texCoord, float2 center, float angle)
{
    float2 origin = texCoord - center;
    origin = float2(
        origin.x * cos(angle) - origin.y * sin(angle),
        origin.x * sin(angle) + origin.y * cos(angle)
    );
    return origin + center;
}
