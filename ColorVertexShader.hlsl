// Data Types (HLSL)
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-data-types

// Shader Constants (HLSL)
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-constants

// Register
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-variable-register

// Constant buffer
// GPU�� �ִ� Register�� �־�� (Constant buffer -> b)
cbuffer ModelViewProjectionConstantBuffer : register(b0) {
    matrix model;
    matrix view;
    matrix projection;
};
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//// �� �������� ConstantBuffer�� ���� ������
//struct ModelViewProjectionConstantBuffer
//{
//    Matrix model;
//    Matrix view;
//    Matrix projection;
//};

// Semantics
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics

// Pixel shader's input == Vertex shader's output
struct VertexShaderInput {
    float3 pos      : POSITION;
    float3 color    : COLOR0;
    float2 texcoord : TEXCOORD0;
};

// System-value semantics for the rasteration stage
// Vertex shader ������ �����Ͱ� �ƴ϶�, Pixel ������ Interpolation�� �� ������ ��
struct PixelShaderInput {
    float4 pos      : SV_POSITION;
    float3 color    : COLOR;
    float2 texcoord : TEXCOORD;
};

// Intrinsic Functions
// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-intrinsic-functions
// �������, mul -> Vector �� ����� ������


// Position & color�� �޾Ƽ� Pixel shader ���� ����Ҽ� �ִ� Position & Color�� ��ȯ����
// Vertex �ϳ��� Vertex shader �ڵ尡 �ѹ��� ȣ�� ��
PixelShaderInput main(VertexShaderInput input) {

    PixelShaderInput output;
    float4 pos = float4(input.pos, 1.0f);

    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;
    output.texcoord = input.texcoord;

    return output;
}
