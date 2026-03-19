// D3D11Renderer.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "D3D11Renderer.h"


// This is an example of an exported variable
D3D11RENDERER_API int nD3D11Renderer=0;

// This is an example of an exported function.
D3D11RENDERER_API int fnD3D11Renderer(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CD3D11Renderer::CD3D11Renderer()
{
    return;
}
