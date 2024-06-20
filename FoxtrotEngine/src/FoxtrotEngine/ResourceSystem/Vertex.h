#pragma once
#include <directxtk/SimpleMath.h>
#include <spine/spine.h>

struct Vertex {
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 color;
    //DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 texcoord;
};

struct SpineVertex {
    // Position in x/y plane
    float x, y;

    // UV coordinates
    float u, v;

    // Color, each channel in the range from 0-1
    // (Should really be a 32-bit RGBA packed color)
    spine::Color color;
};