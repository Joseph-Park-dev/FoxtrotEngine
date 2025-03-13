#pragma once
#include "ResourceSystem/FTResource.h"

#include <wrl.h>
#include <d3d11.h>

class FoxtrotRenderer;

using namespace Microsoft::WRL;

class FTShader :
    public FTResource
{
public:
    virtual void CompileShader(FoxtrotRenderer* renderer) = 0;
};

