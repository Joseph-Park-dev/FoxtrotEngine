
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

// SV_Target -> 결과가 render target 에 저장
float4 main(PixelShaderInput input) : SV_TARGET
{
    return float4(input.color, 1.0);
}
