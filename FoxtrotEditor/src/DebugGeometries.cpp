#include "DebugGeometries.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"

void DebugShape::Initialize(FoxtrotRenderer* renderer)
{
    mesh = new Mesh();
    basicVertexConstantData.model = DirectX::SimpleMath::Matrix();
    basicVertexConstantData.invTranspose = DirectX::SimpleMath::Matrix();
    basicVertexConstantData.view = DirectX::SimpleMath::Matrix();
    basicVertexConstantData.projection = DirectX::SimpleMath::Matrix();

    D3D11Utils::CreateConstantBuffer(renderer->GetDevice(), basicVertexConstantData, mesh->vertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(renderer->GetDevice(), basicPixelConstantData, mesh->pixelConstantBuffer);
}

void DebugShape::Render(
    FoxtrotRenderer* renderer,
    ComPtr<ID3D11VertexShader>& vertexShader,
    ComPtr<ID3D11PixelShader>& pixelShader,
    ComPtr<ID3D11InputLayout>& inputLayout
)
{
    // Set the primitive topology (triangle list)
    renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Bind the vertex buffer to the pipeline
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    renderer->GetContext()->VSSetShader(vertexShader.Get(), 0, 0);
    renderer->GetContext()->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf());
    renderer->GetContext()->PSSetShader(pixelShader.Get(), 0, 0);
    renderer->GetContext()->PSSetConstantBuffers(0, 1,
        mesh->pixelConstantBuffer.GetAddressOf());

    renderer->GetContext()->IASetInputLayout(inputLayout.Get());
    renderer->GetContext()->IASetVertexBuffers(
        0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);

    renderer->GetContext()->Draw(mesh->mVertexCount, 0);
}

void DebugShape::Destroy()
{
    delete mesh;
}

void DrawPolygonFcn(b2Transform transform, const b2Vec2* vertices, int vertexCount, float radius, b2HexColor color, void* context)
{
    DebugGeometries::GetInstance()->DrawPolygon(transform, vertices, vertexCount, color, context);
}

DebugGeometries::DebugGeometries()
    : mVertexShaderPath(L"./assets/Shaders/EditorDebugGeometryVS.hlsl")
    , mPixelShaderPath(L"./assets/Shaders/EditorDebugGeometryPS.hlsl")
{
    mDebugDraw = b2DefaultDebugDraw();
}

DebugGeometries::~DebugGeometries()
{
    for (size_t i = 0; i < mDebugPolygons.size(); ++i)
    {
        mDebugPolygons[i]->Destroy();
        delete mDebugPolygons[i];
    }
    mDebugPolygons.clear();
}

void DebugGeometries::Initialize(FTCoreEditor* coreEditorInst)
{
    mRenderer = coreEditorInst->GetGameRenderer();
    if (mRenderer)
        this->CreateShaders(mRenderer->GetDevice());
    else
        printf("ERROR : DebugGeometries::Initialize()-> Renderer is null");
    mDebugDraw.DrawSolidPolygon = DrawPolygonFcn;
    mDebugDraw.drawShapes = true;
}


void DebugGeometries::Render(FoxtrotRenderer* renderer)
{
    for (size_t i = 0; i < mDebugPolygons.size(); ++i)
        mDebugPolygons[i]->Render(renderer, mVertexShader, mPixelShader, mInputLayout);
}

void DebugGeometries::DrawPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
{
    for (size_t i = 0; i < mDebugPolygons.size(); ++i)
    {
        mDebugPolygons[i]->Update(mRenderer, transform, vertices, vertexCount, color, context);
    }
}

void DebugPolygon::Update(FoxtrotRenderer* renderer, b2Transform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context)
{
    //float lengthA = b2Length(vertices[0] - vertices[1]);
    //float lengthB = b2Length(vertices[1] - vertices[2]);
    //float scaleRatio = 0;
    //if (lengthA > lengthB)
    //    scaleRatio = lengthA / lengthB;
    //else
    //    scaleRatio = lengthB / lengthA;

    FTVector3 posCache = FTVector3(transform.p.x, transform.p.y, 0.f);
    FTVector3 scaleCache = FTVector3(scale->x, scale->y, 1.f);

    DirectX::SimpleMath::Matrix model =
        DirectX::SimpleMath::Matrix::CreateScale(scaleCache.GetDXVec3()) *
        DirectX::SimpleMath::Matrix::CreateRotationZ(b2Rot_GetAngle(transform.q)) *
        DirectX::SimpleMath::Matrix::CreateTranslation(posCache.x, posCache.y, posCache.z);
    basicVertexConstantData.model = model.Transpose();

    basicVertexConstantData.view = Camera::GetInstance()->GetViewRow().Transpose();
    basicVertexConstantData.projection = Camera::GetInstance()->GetProjRow().Transpose();

    D3D11Utils::UpdateBuffer(renderer->GetDevice(), renderer->GetContext(), basicVertexConstantData,
        mesh->vertexConstantBuffer);

    D3D11Utils::UpdateBuffer(renderer->GetDevice(), renderer->GetContext(), basicPixelConstantData,
        mesh->pixelConstantBuffer);
}

void DebugGeometries::DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context)
{
}

void DebugGeometries::CreateShaders(ComPtr<ID3D11Device>& device)
{
    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, 
        mVertexShaderPath, 
        basicInputElements,
        mVertexShader, 
        mInputLayout
    );

    D3D11Utils::CreatePixelShader(
        device, 
        mPixelShaderPath,
        mPixelShader
    );
}

void DebugGeometries::AddPolygons(MeshData meshData, FTVector2* polygonScale)
{
   /* for (size_t i = 0; i < mDebugMeshes.size(); ++i)
        delete mDebugMeshes[i];
    mDebugMeshes.clear();*/

    DebugPolygon* polygon = new DebugPolygon;
    polygon->Initialize(mRenderer);
    polygon->mesh->mVertexCount = meshData.vertices.size();
    D3D11Utils::CreateVertexBuffer(mRenderer->GetDevice(), meshData.vertices, polygon->mesh->vertexBuffer);
    polygon->scale = polygonScale;
    mDebugPolygons.push_back(polygon);
}

void DebugGeometries::B2Vec2ArrayToVertices(const b2Vec2* vertices, int vertexCount, MeshData& meshData)
{
    meshData.vertices.reserve(vertexCount);
    for (size_t i = 0; i < vertexCount; ++i)
        meshData.vertices.push_back(vertices[i]);
}