#include "DebugGeometries.h"

#include "Renderer/D3D11Utils.h"

DebugGeometries::DebugGeometries()
    : mVertexShaderPath(L"./assets/Shaders/EditorDebugGeometryVS.hlsl")
    , mPixelShaderPath(L"./assets/Shaders/EditorDebugGeometryPS.hlsl")
{}

DebugGeometries::~DebugGeometries()
{
    for (FTShape* shape : mShapes)
    {
        if (shape)
            delete shape;
    }
    mShapes.clear();
}

void DebugGeometries::Initialize(FoxtrotRenderer* renderer)
{
    this->CreateShaders(renderer->GetDevice());
}

void DebugGeometries::Render(FoxtrotRenderer* renderer)
{
    if (mShapes.size() < 1)
        return;
    for (FTShape* shape : mShapes)
    {
        if (shape)
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

void DebugGeometries::AddShape(FTShape* shape)
{
    mShapes.push_back(shape);
}

void DebugGeometries::DeleteAll()
{
    if (mShapes.size() < 1)
        return;
    for (FTShape* shape : mShapes)
    {
        if (shape)
            delete shape;
    }
    mShapes.clear();
}

void DebugGeometries::CreateShaders(ComPtr<ID3D11Device>& device)
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
