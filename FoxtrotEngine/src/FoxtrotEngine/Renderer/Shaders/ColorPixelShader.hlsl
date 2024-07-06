Texture2D g_texture0 : register(t0);  //텍스처 => 인덱스가 0인 텍스처라는 의미
//Texture2D g_texture1 : register(t1); //텍스처 => 인덱스가 0인 텍스처라는 의미
SamplerState g_sampler : register(s0);  // 샘플러  (register documentation 참조)

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

// SV_Target -> 결과가 render target 에 저장
float4 main(PixelShaderInput input) : SV_TARGET {
    return float4(input.color, 1.0f);
}


// 렌더링 하는 결과를 만들어내는 곳 -> Pixel Shader
// 그래픽스 파이프라인의 마지막
