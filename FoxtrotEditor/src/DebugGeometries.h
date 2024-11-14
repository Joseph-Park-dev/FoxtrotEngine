#pragma once
#include "box2d/box2d.h"

#include "Core/SingletonMacro.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "FTCoreEditor.h"

class FoxtrotRenderer;

struct DebugShape
{
    void Initialize(FoxtrotRenderer* renderer);
    void Render(
        FoxtrotRenderer* renderer,
        ComPtr<ID3D11VertexShader>& vertexShader,
        ComPtr<ID3D11PixelShader>& pixelShader,
        ComPtr<ID3D11InputLayout>& inputLayout
        );
    void Destroy();

    BasicVertexConstantData     basicVertexConstantData;
    BasicPixelConstantData	    basicPixelConstantData;

    Mesh* mesh;
    bool isInitialized = false;
};

struct DebugPolygon : DebugShape
{
    void Update(FoxtrotRenderer* renderer, b2Transform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
    FTVector2* scale;
};

class DebugGeometries
{
    SINGLETON(DebugGeometries)

public:
            void DrawPolygon(b2Transform transform, const b2Vec2* vertices, int vertexCount, b2HexColor color, void* context);
            void DrawCircle(b2Vec2 center, float radius, b2HexColor color, void* context);

public:
            void AddPolygons(MeshData meshData, FTVector2* polygonScale);

public:
    b2DebugDraw* GetDebugDraw() { return &mDebugDraw; }

public:
            void Initialize(FTCoreEditor* coreEditorInst);
    virtual void Render(FoxtrotRenderer* renderer);

protected:
    virtual	void CreateShaders(ComPtr<ID3D11Device>& device);

private:
    FoxtrotRenderer*            mRenderer;
    b2DebugDraw                 mDebugDraw;

private:
    std::vector<DebugPolygon*>  mDebugPolygons;

private:
    std::wstring                mVertexShaderPath;
    std::wstring                mPixelShaderPath;

    ComPtr<ID3D11VertexShader>  mVertexShader;
    ComPtr<ID3D11PixelShader>   mPixelShader;
    ComPtr<ID3D11InputLayout>   mInputLayout;

private:
    void B2Vec2ArrayToVertices(const b2Vec2* vertices, int vertexCount, MeshData& meshData);
};