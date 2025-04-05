#include "FTRectangle.h"

#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/Camera.h"
#include "Managers/DebugShapes.h"

using Matrix = DirectX::SimpleMath::Matrix;

FTRectArea* FTRectangle::GetRectArea()
{
    return mRectArea;
}

void FTRectangle::Initialize(FoxtrotRenderer* renderer)
{
    FTShape::Initialize(renderer);
    //InitializeMesh(renderer->GetDevice(), GeometryGenerator::MakeSquare());
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
    if (!GetMesh())
        return;
    FTVector3 center = pos + FTVector3(mRectArea->GetCenter());
    FTVector3 size = scale * FTVector3(mRectArea->GetSize());
    Matrix model =
        Matrix::CreateScale(size.GetDXVec3()) *
        Matrix::CreateRotationY(rot.y) *
        Matrix::CreateRotationX(rot.x) *
        Matrix::CreateRotationZ(rot.z) *
        Matrix::CreateTranslation(center.GetDXVec3());
    GetVertexConstantData().model = model.Transpose();
    GetVertexConstantData().view = camInst->GetViewRow().Transpose();
    GetVertexConstantData().projection = camInst->GetProjRow().Transpose();
    
    GetPixelConstantData().IsActive = GetIsActive();
}

void FTRectangle::UpdateConstantBufferModel()
{
    Matrix model =
        Matrix::CreateScale(mRectArea->GetSize().x, mRectArea->GetSize().y, 1.0f) *
        Matrix::CreateTranslation(mRectArea->GetCenter().x, mRectArea->GetCenter().y, 0.0f);
    GetVertexConstantData().model = model.Transpose();
}

FTRectangle::FTRectangle()
    : FTShape()
    , mRectArea(new FTRectArea())
{
}

FTRectangle::~FTRectangle()
{
    delete mRectArea;
}

void FTRectangle::SaveProperties(std::ofstream& ofs)
{
    FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTRECTANGLE);
    FileIOHelper::SaveBool(ofs, ChunkKey::FTSHAPE_IS_ACTIVE, GetIsActive());
    mRectArea->SaveProperties(ofs);
    FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTRECTANGLE);
}

void FTRectangle::LoadProperties(std::ifstream& ifs)
{
    bool isActive = false;

    FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTRECTANGLE);
    mRectArea->LoadProperties(ifs);
    FileIOHelper::LoadBool(ifs, isActive);

    SetIsActive(isActive);
}

#ifdef FOXTROT_EDITOR
void FTRectangle::UpdateUI()
{
    bool isActive = GetIsActive();
    CommandHistory::GetInstance()->UpdateBoolValue("Render Shape", isActive);
    SetIsActive(isActive);
}
#endif