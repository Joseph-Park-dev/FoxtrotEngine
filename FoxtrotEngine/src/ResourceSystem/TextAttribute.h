#pragma once

struct TextAttribute
{
    TextAttribute()
        : Scale(1.f)
        , CharSpacing(0.f)
        , LineSpacing(0.f)
        , MaxChars(10)
    {}

    TextAttribute* operator= (TextAttribute* origin)
    {
        this->Scale = origin->Scale;
        this->CharSpacing = origin->CharSpacing;
        this->LineSpacing = origin->LineSpacing;
        this->MaxChars = origin->MaxChars;
        return this;
    }

    float   Scale;
    float   CharSpacing;
    float   LineSpacing;

    int     MaxChars;
};