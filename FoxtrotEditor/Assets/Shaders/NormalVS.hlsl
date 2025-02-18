#include "Common.hlsli"

cbuffer NormalVertexConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

cbuffer NormalVertexConstantBufer : register(b0)
{
    float scale;
}

float4 main(TexVSInput input) : SV_POSITION
{
    TexPSInput output;
    float4 pos = float4(input.posModel, 1.0f);
    
    // Normal transformation
    float4 normal = float4(input.normalModel, 0.0f);
    output.normalWorld = mul(normal, invTranspose).xyz;
    output.normalWorld = normalize(output.normalWorld);
    
    output.posWorld = mul(pos, model);
    float t = input.texcoord.x;
    pos.xyz = output.normalWorld * t * scale;
    
    output.posWorld = pos.xyz;
    
    pos = mul(pos, view);
    pos = mul(pos, projection);
    output.posProj = pos;
    
    output.texcoord = input.texcoord;
    
    return output;
}