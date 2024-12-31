// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DebugShapes.h"

#include "Renderer/D3D11Utils.h"

DebugShapes::DebugShapes()
    : mVertexShaderPath(L"./Assets/Shaders/EditorDebugShapeVS.hlsl")
    , mPixelShaderPath(L"./Assets/Shaders/EditorDebugShapePS.hlsl")
{}

DebugShapes::~DebugShapes()
{
    if(0 < mShapes.size())
        mShapes.clear();
}

void DebugShapes::Initialize(FoxtrotRenderer* renderer)
{
    this->CreateShaders(renderer->GetDevice());
}

void DebugShapes::Render(FoxtrotRenderer* renderer)
{
    if (mShapes.size() < 1)
        return;
    for (FTShape* shape : mShapes)
    {
        if (shape->GetIsActive())
        {
            shape->Render
            (
                renderer,
                mVertexShader,
                mPixelShader,
                mInputLayout
            );
        }
    }
}

void DebugShapes::AddShape(FTShape* shape)
{
    mShapes.push_back(shape);
}

void DebugShapes::RemoveShape(FTShape* shape)
{
    auto iter = std::find(mShapes.begin(), mShapes.end(), shape);
    if (iter != mShapes.end())
    {
        delete shape;
        mShapes.erase(iter);
    }
}

void DebugShapes::DeleteAll()
{
    if (mShapes.size() < 1)
        return;
    for (FTShape* shape : mShapes)
    {
        if (shape)
        {
            delete shape;
            shape = nullptr;
        }
    }
    mShapes.clear();
}

void DebugShapes::CreateShaders(ComPtr<ID3D11Device>& device)
{
    std::vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
     D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
     D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, mVertexShaderPath, basicInputElements,
        mVertexShader, mInputLayout);

    D3D11Utils::CreatePixelShader(
        device, mPixelShaderPath,
        mPixelShader);
}
