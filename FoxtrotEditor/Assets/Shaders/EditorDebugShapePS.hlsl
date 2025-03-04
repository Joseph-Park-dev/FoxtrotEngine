// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Common.hlsli"
cbuffer PixelConstantBuffer : register(b0)
{
    float4 indexColor;
};

// SV_Target -> 결과가 render target 에 저장
IndexPSOutput main(SolidPSInput input)
{
    IndexPSOutput output;
    output.pixelColor = input.color;
    output.indexColor = indexColor;
    
    return output;
}
