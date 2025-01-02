// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

// Pixel shader's input == Vertex shader's output
struct VertexShaderInput
{
    float3 pos : POSITION0;
    float3 color : COLOR0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    
    return output;
}