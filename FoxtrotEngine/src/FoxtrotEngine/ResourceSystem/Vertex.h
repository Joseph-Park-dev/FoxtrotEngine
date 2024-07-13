#pragma once
#include "directxtk/SimpleMath.h"

#include "FoxtrotEngine/Math/FTMath.h"
struct Vertex {
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 color;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 texcoord;
};