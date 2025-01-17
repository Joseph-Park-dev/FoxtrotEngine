#pragma once

struct TextAttribute
{
    TextAttribute()
        : Size(0.f)
        , CharSpacing(0.f)
        , LineSpacing(0.f)
        , MaxChars(0)
    {}

    float   Size;
    float   CharSpacing;
    float   LineSpacing;

    size_t  MaxChars;
};