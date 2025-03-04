#include "FTRectangle.h"

#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
#include "DebugShapes.h"
#endif // FOXTROT_EDITOR


using Matrix = DirectX::SimpleMath::Matrix;

FTRectangle::FTRectangle()
    : FTShape()
    , mRectArea(new FTRectArea())
{
}

FTRectangle::~FTRectangle()
{
    delete mRectArea;
}

FTRectArea* FTRectangle::GetRectArea()
{
    return mRectArea;
}

void FTRectangle::Initialize(FoxtrotRenderer* renderer)
{
    FTShape::Initialize(renderer);
    InitializeMesh(renderer->GetDevice(), GeometryGenerator::MakeSquare());

#ifdef FOXTROT_EDITOR
    DebugShapes::GetInstance()->AddShape(this);
#endif // FOXTROT_EDITOR
}

void FTRectangle::Update()
{
    if (!GetMesh())
        return;
    UpdateConstantBufferModel();
    UpdateConstantBufferView(Camera::GetInstance());
    UpdateConstantBufferProjection(Camera::GetInstance());
}

void FTRectangle::Update(FTVector3 pos, FTVector3 rot, FTVector3 scale, Camera* camInst)
{
    Matrix model =
        Matrix::CreateScale(scale.GetDXVec3()) *
        Matrix::CreateRotationY(rot.y) *
        Matrix::CreateRotationX(rot.x) *
        Matrix::CreateRotationZ(rot.z) *
        Matrix::CreateTranslation(pos.GetDXVec3());
    GetVertexConstantData().model = model.Transpose();
    GetVertexConstantData().view = camInst->GetViewRow().Transpose();
    GetVertexConstantData().projection = camInst->GetProjRow().Transpose();
}

void FTRectangle::UpdateConstantBufferModel()
{
    Matrix model =
        Matrix::CreateScale(mRectArea->GetSize().x, mRectArea->GetSize().y, 1.0f) *
        Matrix::CreateTranslation(mRectArea->GetCenter().x, mRectArea->GetCenter().y, 0.0f);
    GetVertexConstantData().model = model.Transpose();
}