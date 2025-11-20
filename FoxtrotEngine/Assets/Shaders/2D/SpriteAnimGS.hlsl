#include "SpriteAnim.hlsli"

cbuffer GSConst : register(b0)
{
    // Size of animation on screen.
    float2 size;
    
    // Is the frame rotated by 90 degrees?
    bool rotated;
    
    // Dummy variable keep buffer 16 byte aligned.
    float dummy;
    
    // X ,Y coordinates, and size on Sprite sheet (ranged from 0 to 1).
    float4 frame;
    
}

[maxvertexcount(5)]
void main(
	point GS_IN input[1] : SV_POSITION,
	inout LineStream<PS_IN> outStream
)
{
    float2 sizeVec = size * 0.5;
    float4 right = input[0].right;
    float4 up = float4(-right.y, right.x, 0.0, 0.0);
    
    float4 topLeft = input[0].posWorld - sizeVec.x * right + sizeVec.y * up;
    float4 topRight = input[0].posWorld + sizeVec.x * right + sizeVec.y * up;
    float4 bottomLeft = input[0].posWorld - sizeVec.x * right - sizeVec.y * up;
    float4 bottomRight = input[0].posWorld + sizeVec.x * right - sizeVec.y * up;
    
    PS_IN output;
    
    ////////////////////////////
    //////// Top Left //////////
    ////////////////////////////
    
    output.posProj = topLeft;
    
    if (rotated)
        output.texCoord = frame.xy + frame.w;
    else
        output.texCoord = frame.xy;
    
    outStream.Append(output);
    
    /////////////////////////////
    //////// Top Right //////////
    /////////////////////////////
    
    output.posProj = topRight;
    
    if (rotated)
        output.texCoord = frame.xy + frame.zw;
    else
        output.texCoord = frame.xy + frame.z;
    
    outStream.Append(output);
    
    ////////////////////////////////
    //////// Bottom Right //////////
    ////////////////////////////////
    
    output.posProj = bottomRight;

    if(rotated)
        output.texCoord = frame.xy + frame.w;
    else 
        output.texCoord = frame.xy + frame.zw;
    
    outStream.Append(output);
    
    ///////////////////////////////
    //////// Bottom Left //////////
    ///////////////////////////////
    
    output.posProj = bottomLeft;
    
   if(rotated)
        output.texCoord = frame.xy;
    else
        output.texCoord = frame.xy + frame.zw;
    
    outStream.Append(output);
    
    ////////////////////////////
    //////// Top Left //////////
    ////////////////////////////
    
    output.posProj = topLeft;
    
    if (rotated)
        output.texCoord = frame.xy + frame.w;
    else
        output.texCoord = frame.xy;
    
    outStream.Append(output);
}