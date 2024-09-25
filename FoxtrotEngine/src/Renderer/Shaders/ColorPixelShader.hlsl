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

cbuffer TileMapConstantBuffer : register(b0)
{
    // 타일 하나의 시작점 (왼쪽 위) X, Y 좌표
    float tileX;     
    float tileY;
    
    // 타일 하나의 X, Y 사이즈
    float tileSizeX;
    float tileSizeY;
}

// 텍스처 샘플링 좌표가 타일 범위를 넘어갈시 업데이트
static float2 texcoord = float2(0.0, 0.0);
static float2 offset = float2(0.0, 0.0);

// SV_Target -> 결과가 render target 에 저장
float4 main(PixelShaderInput input) : SV_TARGET {
    // tileCoord : 타일 내부 좌표
    
    //// 텍스처 샘플링 좌표
    //float x = 0.0;
    //float y = 0.0;
    
    //// 텍스처 샘플링 좌표 계산
    
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

// 렌더링 하는 결과를 만들어내는 곳 -> Pixel Shader
// 그래픽스 파이프라인의 마지막
