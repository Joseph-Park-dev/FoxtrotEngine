cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

// Semantics
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics

// Pixel shader's input == Vertex shader's output
struct VertexShaderInput
{
    float3 pos : POSITION0;
    float3 color : COLOR0;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

// System-value semantics for the rasteration stage
// Vertex shader 단위의 데이터가 아니라, Pixel 단위로 Interpolation이 된 정보가 들어감
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

// Intrinsic Functions
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-intrinsic-functions
// 예를들어, mul -> Vector 와 행렬을 곱해줌


// Position & color를 받아서 Pixel shader 에서 사용할수 있는 Position & Color를 반환해줌
// Vertex 하나당 Vertex shader 코드가 한번씩 호출 됨
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    
    return output;
}