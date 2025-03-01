#pragma once
#include <DirectXColors.h>

using Vector2 = DirectX::SimpleMath::Vector2;
using Vector4 = DirectX::SimpleMath::Vector4;

struct TextAttribute
{
    TextAttribute()
        : Rotation  (0.0f)
        , Offset    (Vector2(0.0f, 0.0f))
        , Origin    (Vector2(0.0f,0.0f))
        , Scale     (Vector2(0.0f, 0.0f))
        , Color     (DirectX::Colors::Black)
    {}

    TextAttribute* operator= (TextAttribute* origin)
    {
        this->Rotation  = origin->Rotation;
        this->Offset    = origin->Offset;
        this->Color     = origin->Color;
        this->Origin    = origin->Origin;
        this->Scale     = origin->Scale;
        return this;
    }

    float   Rotation;
    Vector2 Offset;
    Vector2 Origin;
    Vector2 Scale;
    Vector4 Color;
};