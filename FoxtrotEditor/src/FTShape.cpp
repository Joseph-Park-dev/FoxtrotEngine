#include "FTShape.h"

#include "Renderer/D3D11Utils.h"

#include "Core/TemplateFunctions.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"
#include "DebugGeometries.h"

using DXMatrix = DirectX::SimpleMath::Matrix;

FTShape::FTShape()
    : mMesh(nullptr)
    , mVertexConstantData()
    , mPixelConstantData()
{
}

FTShape::~FTShape()
{
    if (mMesh)
    {
        delete mMesh;
        mMesh = nullptr;
    }
    if (mVertexConstantBuffer.Get())
        mVertexConstantBuffer.Reset();
    if (mPixelConstantBuffer.Get())
        mPixelConstantBuffer.Reset();
}

void FTShape::Initialize(FoxtrotRenderer* renderer)
{
    InitializeConstantBuffer(renderer->GetDevice());
}

void FTShape::Update(Transform* transform, Camera* cameraInst)
{
    if (!mMesh)
        return;
    UpdateConstantBufferModel(transform);
    UpdateConstantBufferView(cameraInst);
    UpdateConstantBufferProjection(cameraInst);
}

void FTShape::Update(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* cameraInst)
{
    if (!mMesh)
        return;
    UpdateConstantBufferModel(pos, rot, scale);
    UpdateConstantBufferView(cameraInst);
    UpdateConstantBufferProjection(cameraInst);
}

void FTShape::Render(
    FoxtrotRenderer* renderer, 
    ComPtr<ID3D11VertexShader>& vertexShader,
    ComPtr<ID3D11PixelShader>& pixelShader,
    ComPtr<ID3D11InputLayout>& inputLayout
)
{
    if (!mMesh)
        return;
    if (!mVertexConstantBuffer.Get())
        return;
    if (!mPixelConstantBuffer.Get())
        return;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    size_t meshSize = GetArrayLength<Mesh>(mMesh);

    UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

    ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
    for (size_t i = 0; i < meshSize; ++i)
    {
        context->VSSetShader(vertexShader.Get(), 0, 0);
        context->VSSetConstantBuffers(0, 1, mMesh[i].vertexConstantBuffer.GetAddressOf());

        context->PSSetShader(pixelShader.Get(), 0, 0);
        context->PSSetConstantBuffers(0, 1,
            mMesh[i].pixelConstantBuffer.GetAddressOf());

        context->IASetInputLayout(inputLayout.Get());
        context->IASetVertexBuffers(0, 1, mMesh[i].vertexBuffer.GetAddressOf(),
            &stride, &offset);
        context->IASetIndexBuffer(mMesh[i].indexBuffer.Get(), DXGI_FORMAT_R32_UINT,
            0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mMesh[i].mIndexCount, 0, 0);
    }
}

void FTShape::InitializeMesh(ComPtr<ID3D11Device>& device, MeshData&& meshData)
{
    mMesh = DBG_NEW Mesh;
    mMesh->mIndexCount = UINT(meshData.indices.size());
    mMesh->mVertexCount = UINT(meshData.vertices.size());

    D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
        mMesh->vertexBuffer);
    D3D11Utils::CreateIndexBuffer(device, meshData.indices,
        mMesh->indexBuffer);

    mMesh->vertexConstantBuffer = mVertexConstantBuffer;
    mMesh->pixelConstantBuffer = mPixelConstantBuffer;
}

void FTShape::UpdateConstantBufferModel(Transform* transform)
{
    int dir = transform->GetCurrentDirection();
    FTVector3 worldPos = FTVector3(
        transform->GetWorldPosition().x,
        transform->GetWorldPosition().y,
        transform->GetWorldPosition().z
    );
    FTVector3 scale = transform->GetScale();
    DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x * dir, scale.y, scale.z);

    Matrix model =
        DXMatrix::CreateScale(scaleWithDir) *
        DXMatrix::CreateRotationY(transform->GetRotation().y) *
        DXMatrix::CreateRotationX(transform->GetRotation().x) *
        DXMatrix::CreateRotationZ(transform->GetRotation().z) *
        DXMatrix::CreateTranslation(worldPos.GetDXVec3());
    mVertexConstantData.model = model.Transpose();
}

void FTShape::UpdateConstantBufferModel(FTVector3 pos, FTVector3 rot, FTVector3 scale)
{
    Matrix model =
        DXMatrix::CreateScale(scale.GetDXVec3()) *
        DXMatrix::CreateRotationY(rot.y) *
        DXMatrix::CreateRotationX(rot.x) *
        DXMatrix::CreateRotationZ(rot.z) *
        DXMatrix::CreateTranslation(pos.GetDXVec3());
    GetVertexConstantData().model = model.Transpose();
}

void FTShape::UpdateConstantBufferView(Camera* camInst)
{
    mVertexConstantData.view = camInst->GetViewRow().Transpose();
}

void FTShape::UpdateConstantBufferProjection(Camera* camInst)
{
    mVertexConstantData.projection = camInst->GetProjRow().Transpose();
}

void FTShape::InitializeConstantBuffer(ComPtr<ID3D11Device>& device)
{
    mVertexConstantData.model = DirectX::SimpleMath::Matrix();
    mVertexConstantData.view = DirectX::SimpleMath::Matrix();
    mVertexConstantData.projection = DirectX::SimpleMath::Matrix();

    D3D11Utils::CreateConstantBuffer(device, mVertexConstantData, mVertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(device, mPixelConstantData, mPixelConstantBuffer);
}

void FTShape::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    if (mVertexConstantBuffer)
        D3D11Utils::UpdateBuffer(device, context, mVertexConstantData,
            mVertexConstantBuffer);
    else
        printf("ERROR : FTShape::UpdateConstantBuffers() -> Vertex Constant Buffer is null");

    if(mPixelConstantBuffer)
        D3D11Utils::UpdateBuffer(device, context, mPixelConstantData,
            mPixelConstantBuffer);
    else 
        printf("ERROR : FTShape::UpdateConstantBuffers() -> Pixel Constant Buffer is null");
}

FTRectangle::FTRectangle()
    : FTShape()
    , mSize(1.f, 1.f)
{
}

void FTRectangle::Initialize(FoxtrotRenderer* renderer)
{
    FTShape::Initialize(renderer);
    InitializeMesh(renderer->GetDevice(), GeometryGenerator::MakeSquare());
    DebugGeometries::GetInstance()->AddShape(this);
}

void FTRectangle::UpdateConstantBufferModel(FTVector3 pos, FTVector3 rot, FTVector3 scale)
{
    FTVector3 modifiedScale = FTVector3(mSize.x * scale.x, mSize.y * scale.y, 1.0f);
    Matrix model =
        DXMatrix::CreateScale(modifiedScale.GetDXVec3()) *
        DXMatrix::CreateRotationY(rot.y) *
        DXMatrix::CreateRotationX(rot.x) *
        DXMatrix::CreateRotationZ(rot.z) *
        DXMatrix::CreateTranslation(pos.GetDXVec3());
    GetVertexConstantData().model = model.Transpose();
}
