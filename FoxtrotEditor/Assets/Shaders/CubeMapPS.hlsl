#include "Common.hlsli"

TextureCube g_diffuseCube : register(t0);
TextureCube g_specularCube : register(t1);
SamplerState g_sampler : register(s0);

float4 main(TexPSInput input) : SV_TARGET
{
    return g_specularCube.Sample(g_sampler, input.posWorld.xyz);
}