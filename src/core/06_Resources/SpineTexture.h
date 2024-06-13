#pragma once
#include <spine/TextureLoader.h>
#include <spine/Extension.h>
#include "FTTexture.h"

class SpineTexture :
    public FTTexture, public spine::TextureLoader{
public:
    //virtual bool load(spine::AtlasPage& page, const spine::String& path);

public:
    SpineTexture()
        : FTTexture()
    {

    }
    ~SpineTexture() override;
};