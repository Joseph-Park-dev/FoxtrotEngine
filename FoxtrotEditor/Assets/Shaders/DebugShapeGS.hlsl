#include "Common.hlsli"

cbuffer ModelConst : register(b0)
{
    matrix model;
}

cbuffer ShapeConst : register(b1)
{
    matrix view;
    matrix proj;
    float2 size;
    float2 dummy;
}

//struct Line
//{
//    float2 pos0;
//    float2 pos1;
//};

//struct WeightedLine
//{
//    Line upper;
//    Line lower;
//};

//WeightedLine GetWeightedLine(float4 pos0, float4 pos1)
//{
//    float2 direction = normalize(pos1.xy - pos0.xy);
//    float2 perp = float2(-direction.y, direction.x) * (lineWidth * 0.5);
    
//    WeightedLine lineSeg;
    
//    lineSeg.upper.pos0 = pos0;
//    lineSeg.upper.pos1 = pos1;
    
//    lineSeg.lower.pos0 = pos0;
//    lineSeg.lower.pos1 = pos1;

//    lineSeg.upper.pos0 -= perp;
//    lineSeg.upper.pos1 -= perp;

//    lineSeg.lower.pos0 += perp;
//    lineSeg.lower.pos1 += perp;
    
//    return lineSeg;
//}

[maxvertexcount(5)]
void main(
	point DebugGSIN input[1] : SV_POSITION,
	inout LineStream<DebugPSInput> outStream
)
{
    float2 sizeVec = size * 0.5;
    float4 right = input[0].right;
    float4 up = float4(-right.y, right.x, 0.0, 0.0);
    
    float4 topLeft = input[0].pos - sizeVec.x * right + sizeVec.y * up;
    float4 topRight = input[0].pos + sizeVec.x * right + sizeVec.y * up;
    float4 bottomLeft = input[0].pos - sizeVec.x * right - sizeVec.y * up;
    float4 bottomRight = input[0].pos + sizeVec.x * right - sizeVec.y * up;
    
    topLeft = mul(topLeft, view);
    topLeft = mul(topLeft, proj);
    
    topRight = mul(topRight, view);
    topRight = mul(topRight, proj);
    
    bottomLeft = mul(bottomLeft, view);
    bottomLeft = mul(bottomLeft, proj);
    
    bottomRight = mul(bottomRight, view);
    bottomRight = mul(bottomRight, proj);
    
    DebugPSInput output;
    output.color = input[0].color;
    output.pos = topLeft;
    outStream.Append(output);
    
    output.pos = topRight;
    outStream.Append(output);
    
    output.pos = bottomRight;
    outStream.Append(output);
    
    output.pos = bottomLeft;
    outStream.Append(output);
    
    output.pos = topLeft;
    outStream.Append(output);
}