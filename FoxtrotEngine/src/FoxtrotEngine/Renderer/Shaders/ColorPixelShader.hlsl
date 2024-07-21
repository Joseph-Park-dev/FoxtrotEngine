Texture2D g_texture0 : register(t0);  //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
//Texture2D g_texture1 : register(t1); //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
SamplerState g_sampler : register(s0);  // ���÷�  (register documentation ����)

cbuffer TileMapConstantBuffer : register(b0)
{
    float tileSamplePosX;
    float tileSamplePosY;
    float tileWidth;
    float tileHeight;
}

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

// SV_Target -> ����� render target �� ����
float4 main(PixelShaderInput input) : SV_TARGET {
    //return float4(input.color, 1.0f);
    float2 tileMapCoord = float2(tileSamplePosX, tileSamplePosY);
    float2 tileArea     = float2(tileWidth, tileHeight);
    
    float2 sampleCoord = input.texcoord + tileMapCoord;
    return g_texture0.Sample(g_sampler, sampleCoord);
}


// ������ �ϴ� ����� ������ �� -> Pixel Shader
// �׷��Ƚ� ������������ ������
