#include "SpriteAnim.hlsli"

// View, Project const buffer
cbuffer VPConst : register(b0)
{
    matrix viewMat;
    matrix projMat;
}

cbuffer GSConst : register(b1)
{
    // Size of animation on screen.
    float2 size;
    
    // Scale of animation on screen.
    float2 scale;
    
    // X ,Y coordinates, and size on Sprite sheet (ranged from 0 to 1).
    float4 frame;
    
}

[maxvertexcount(6)]
void main(
	point GS_IN input[1] : SV_POSITION,
	inout TriangleStream<PS_IN> outStream
)
{
    float2 sizeVec = size * 0.5;
    float4 right = input[0].right;
    float4 up = float4(-right.y, right.x, 0.0, 0.0);
    
    float4 topLeft = input[0].posWorld - sizeVec.x * right + sizeVec.y * up;
    float4 topRight = input[0].posWorld + sizeVec.x * right + sizeVec.y * up;
    float4 bottomLeft = input[0].posWorld - sizeVec.x * right - sizeVec.y * up;
    float4 bottomRight = input[0].posWorld + sizeVec.x * right - sizeVec.y * up;
    
    topLeft.xy *= scale;
    topRight.xy *= scale;
    bottomLeft.xy *= scale;
    bottomRight.xy *= scale;
    
    topLeft = mul(topLeft, viewMat);
    topLeft = mul(topLeft, projMat);
    
    topRight = mul(topRight, viewMat);
    topRight = mul(topRight, projMat);
    
    bottomLeft = mul(bottomLeft, viewMat);
    bottomLeft = mul(bottomLeft, projMat);
    
    bottomRight = mul(bottomRight, viewMat);
    bottomRight = mul(bottomRight, projMat);
    
    PS_IN output;
    ////////////////////////////
    //////// Top Left //////////
    ////////////////////////////
    output.posProj = topLeft;
    
    output.texCoord = frame.xy;
    
    //output.texCoord = uvs[0];
    outStream.Append(output);
    
    ///////////////////////////////
    //////// Bottom Left //////////
    ///////////////////////////////
    output.posProj = bottomLeft;
    output.texCoord = frame.xy + float2(0.0, frame.w);
    
    //output.texCoord = uvs[2];
    outStream.Append(output);
    
    /////////////////////////////
    //////// Top Right //////////
    /////////////////////////////
    
    output.posProj = topRight;
    output.texCoord = frame.xy + float2(frame.z, 0.0);
    
    //output.texCoord = uvs[1];
    outStream.Append(output);
    
    outStream.RestartStrip();
    
    ///////////////////////////////
    //////// Bottom Left //////////
    ///////////////////////////////
    output.posProj = bottomLeft;
    output.texCoord = frame.xy + float2(0.0, frame.w);
    
    //output.texCoord = uvs[2];
    outStream.Append(output);
    
    ////////////////////////////////
    //////// Bottom Right //////////
    ////////////////////////////////
    output.posProj = bottomRight;

    output.texCoord = frame.xy;
    output.texCoord.x += frame.z;
    output.texCoord.y += frame.w;
   // output.texCoord = uvs[3];
    
    outStream.Append(output);
    
    /////////////////////////////
    //////// Top Right //////////
    /////////////////////////////
    output.posProj = topRight;
    output.texCoord = frame.xy + float2(frame.z, 0.0);
    
   // output.texCoord = uvs[1];
    
    outStream.Append(output);
    
    outStream.RestartStrip();
}