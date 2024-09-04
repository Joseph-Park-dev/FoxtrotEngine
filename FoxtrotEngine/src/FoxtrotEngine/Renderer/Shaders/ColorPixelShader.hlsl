Texture2D g_texture0 : register(t0);  //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
//Texture2D g_texture1 : register(t1); //�ؽ�ó => �ε����� 0�� �ؽ�ó��� �ǹ�
SamplerState g_sampler : register(s0);  // ���÷�  (register documentation ����)

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

cbuffer TileMapConstantBuffer : register(b0)
{
    // Ÿ�� �ϳ��� ������ (���� ��) X, Y ��ǥ
    float tileX;     
    float tileY;
    
    // Ÿ�� �ϳ��� X, Y ������
    float tileSizeX;
    float tileSizeY;
}

// �ؽ�ó ���ø� ��ǥ�� Ÿ�� ������ �Ѿ�� ������Ʈ
static float2 texcoord = float2(0.0, 0.0);
static float2 offset = float2(0.0, 0.0);

// SV_Target -> ����� render target �� ����
float4 main(PixelShaderInput input) : SV_TARGET {
    // tileCoord : Ÿ�� ���� ��ǥ
    
    //// �ؽ�ó ���ø� ��ǥ
    //float x = 0.0;
    //float y = 0.0;
    
    //// �ؽ�ó ���ø� ��ǥ ���
    
    //x = float(tileX + tileCoord.x);
    //y = float(tileY + tileCoord.y);
    
    //if (tileSizeX <= tileCoord.x)   
    //    offset.x = input.texcoord.x;
    
    //if (tileSizeY <= tileCoord.y)
    //    offset.y = input.texcoord.y;
    
    if (offset.x < tileSizeX)
        offset.x += (input.texcoord.x - texcoord.x);
    else
        texcoord.x = tileX;
    
    if (offset.y < tileSizeY)
        offset.y += (input.texcoord.y - texcoord.y);
    else
        texcoord.y = tileY;

    
    return g_texture0.Sample(g_sampler, input.texcoord);
    //return g_texture0.Sample(g_sampler, input.texcoord);
}

// ������ �ϴ� ����� ������ �� -> Pixel Shader
// �׷��Ƚ� ������������ ������
