#include "Common.hlsli"

TextureCube g_textureCube : register(t0);
SamplerState g_sampler : register(s0);

float4 main(TexPSInput input) : SV_TARGET
{
    return g_textureCube.Sample(g_sampler, input.posWorld.xyz);
}