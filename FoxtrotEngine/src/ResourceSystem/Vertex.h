#pragma once
#include "directxtk/SimpleMath.h"

#include "Math/FTMath.h"
struct Vertex {
    Vertex() {}

    Vertex(float x, float y) 
    {
        position.x = x;
        position.y = y;
    }

    Vertex(b2Vec2 vec2)
    {
        position.x = vec2.x;
        position.y = vec2.y;
    }

    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 color;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 texcoord;
};