#pragma once

struct TextAttribute
{
    TextAttribute()
        : Scale(1.f)
        , CharSpacing(0.f)
        , LineSpacing(0.f)
        , MaxChars(0)
    {}

    float   Scale;
    float   CharSpacing;
    float   LineSpacing;

    size_t  MaxChars;
};