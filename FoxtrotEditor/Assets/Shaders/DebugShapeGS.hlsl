#include "Common.hlsli"

cbuffer MatrixConst : register(b0)
{
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix proj;
}

cbuffer ShapeConst : register(b1)
{
    float2 size;
    float lineWidth;
    float dummy;
};

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
    float2 hw = size * 0.5;
    
    float4 topLeft = input[0].pos + float4(-hw.x, hw.y, 0.0, 0.0);
    float4 topRight = input[0].pos + float4(hw.x, hw.y, 0.0, 0.0);
    float4 bottomLeft = input[0].pos + float4(-hw.x, -hw.y, 0.0, 0.0);
    float4 bottomRight = input[0].pos + float4(hw.x, -hw.y, 0.0, 0.0);
    
    topLeft = mul(topLeft, model);
    topLeft = mul(topLeft, view);
    topLeft = mul(topLeft, proj);
    
    topRight = mul(topRight, model);
    topRight = mul(topRight, view);
    topRight = mul(topRight, proj);
    
    bottomLeft = mul(bottomLeft, model);
    bottomLeft = mul(bottomLeft, view);
    bottomLeft = mul(bottomLeft, proj);
    
    bottomRight = mul(bottomRight, model);
    bottomRight = mul(bottomRight, view);
    bottomRight = mul(bottomRight, proj);
    
   // WeightedLine lineSeg = GetWeightedLine(topLeft, topRight);
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
    
    //output.pos = float4(lineSeg.upper.pos0.xy, 0.0, 0.0);
    //outStream.Append(output);
    //output.pos = float4(lineSeg.upper.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //output.pos = float4(lineSeg.lower.pos0.xy, 0.0, 0.0);
    //outStream.Append(output);
    //output.pos = float4(lineSeg.lower.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);

    //lineSeg = GetWeightedLine(topRight, bottomRight);
    //output.pos0 = float4(lineSeg.upper.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.upper.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //output.pos0 = float4(lineSeg.lower.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.lower.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //lineSeg = GetWeightedLine(bottomRight, bottomLeft);
    //output.pos0 = float4(lineSeg.upper.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.upper.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //output.pos0 = float4(lineSeg.lower.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.lower.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //lineSeg = GetWeightedLine(bottomLeft, topLeft);
    //output.pos0 = float4(lineSeg.upper.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.upper.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
    
    //output.pos0 = float4(lineSeg.lower.pos0.xy, 0.0, 0.0);
    //output.pos1 = float4(lineSeg.lower.pos1.xy, 0.0, 0.0);
    //outStream.Append(output);
}