// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Vertex Data in DirectX 11 context.
/// </summary>

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

#ifdef FOXTROT_EDITOR
    Vertex(b2Vec2 vec2)
    {
        position.x = vec2.x;
        position.y = vec2.y;
    }
#endif // FOXTROT_EDITOR

    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 color;
    DirectX::SimpleMath::Vector3 normal;
    DirectX::SimpleMath::Vector2 texcoord;
};