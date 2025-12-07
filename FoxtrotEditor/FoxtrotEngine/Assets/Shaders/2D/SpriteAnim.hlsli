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
    float4 posWorld : SV_POSITION;
    float4 right : POSITION0;
};

struct PS_IN
{
    float4 posProj : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

float2 rotateUV(float2 uv, float2 pivot, float rotation)
{
    float sine = sin(rotation);
    float cosine = cos(rotation);
    
    // Translate UV coordinates to center the pivot at the origin
    uv -= pivot;
    
    // Apply rotation matrix
    float2 rotatedUV;
    rotatedUV.x = uv.x * cosine - uv.y * sine;
    rotatedUV.y = uv.x * sine + uv.y * cosine;
    
    // Translate back to the original coordinate space
    uv = rotatedUV + pivot;
    
    return uv;
}