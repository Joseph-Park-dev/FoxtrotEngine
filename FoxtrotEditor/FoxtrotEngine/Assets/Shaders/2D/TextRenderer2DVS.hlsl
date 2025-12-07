struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color : COLOR;
    float4 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input, uint vertexID : SV_VertexID)
{
    VS_OUTPUT output;

    // vert id 0 = 0000, uv = (0, 0)
    // vert id 1 = 0001, uv = (1, 0)
    // vert id 2 = 0010, uv = (0, 1)
    // vert id 3 = 0011, uv = (1, 1)
    float x = vertexID & 1;
    float y = (vertexID >> 1) & 1;
    float2 uv = float2(x, y);

    // set the position for the vertex based on which vertex it is (uv)
    output.pos = float4(input.pos.x + (input.pos.z * uv.x), input.pos.y - (input.pos.w * uv.y), 0, 1);
    output.color = input.color;

    // set the texture coordinate based on which vertex it is (uv)
    output.texCoord = float2(input.texCoord.x + (input.texCoord.z * uv.x), input.texCoord.y + (input.texCoord.w * uv.y));

    return output;
}