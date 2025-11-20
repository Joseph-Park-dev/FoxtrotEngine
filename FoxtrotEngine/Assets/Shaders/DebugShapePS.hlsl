// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Common.hlsli"

float alphaThres = 0.05;

cbuffer PixelConstantBuffer : register(b0)
{
    bool isActive;
    bool3 dummy;
};

float4 main(DebugPSInput input) : SV_TARGET
{
    float4 result = isActive ? input.color : float4(0.0,0.0,0.0,0.0);
    if (result.w < alphaThres)
        clip(-1); // Discards the pixel
    return result;
}
