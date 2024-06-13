Texture2D g_texture0 : register(t0);  //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
//Texture2D g_texture1 : register(t1); //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
SamplerState g_sampler : register(s0);  // ���÷�  (register documentation ����)

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float2 texcoord : TEXCOORD;
};

// SV_Target -> ����� render target �� ����
float4 main(PixelShaderInput input) : SV_TARGET {
    //float x = input.texcoord.x;
    //float y = input.texcoord.y;
    //float f = (x - 0.5) * (x - 0.5) + (y - 0.5) * (y - 0.5) - 0.3 * 0.3;
    
    //if(f < 0.0)
    //{
    //    float4 color = g_texture0.Sample(g_sampler, input.texcoord);
    //    return color * 1.5f;
    //}
    //else
    //
    //return input.texcoord.x > xSplit
    //           ? g_texture0.Sample(g_sampler, input.texcoord)
    //            : g_texture1.Sample(g_sampler, input.texcoord);
    
    //return float4(input.color, 1.0);
    return g_texture0.Sample(g_sampler, input.texcoord);
}


// ������ �ϴ� ����� ������ �� -> Pixel Shader
// �׷��Ƚ� ������������ ������
