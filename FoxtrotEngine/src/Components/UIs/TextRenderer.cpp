#include "Components/UIs/TextRenderer.h"

#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"

using DirectX::DX11::SpriteSortMode;

std::string TextRenderer::GetName() const
{
	return "TextRenderer";
}

void TextRenderer::Initialize(FTCore* coreInstance)
{
    FoxtrotRenderer* renderer = coreInstance->GetGameRenderer();
    spriteBatch = new DirectX::SpriteBatch(renderer->GetContext().Get());
    spriteFont = new DirectX::SpriteFont(
        renderer->GetDevice().Get(),
        L"D:/[2024_01]/Test_Project_FTEngine/Assets/Nanum.spritefont"
    );
}

void TextRenderer::Render(FoxtrotRenderer* renderer)
{
    FTVector2 pos = GetOwner()->GetTransform()->GetWorldPosition();

    renderer->GetContext()->OMSetDepthStencilState(renderer->GetDSS2D().Get(), 0);
    spriteBatch->Begin(SpriteSortMode::SpriteSortMode_Deferred);
    spriteFont->DrawString(
        spriteBatch, mText.c_str(),
        pos.GetD3Vec2() + mTextAttribute->Offset,
        mTextAttribute->Color,
        mTextAttribute->Rotation,
        mTextAttribute->Origin,
        mTextAttribute->Scale
    );
    spriteBatch->End();
    renderer->GetContext()->OMSetDepthStencilState(renderer->GetDSS().Get(), 0);
}

void TextRenderer::CloneTo(Actor* actor)
{
    TextRenderer* newComp = DBG_NEW TextRenderer(actor, GetUpdateOrder());

    newComp->mText = this->mText;
    newComp->mTextAttribute->Rotation   = mTextAttribute->Rotation;
    newComp->mTextAttribute->Offset     = mTextAttribute->Offset;
    newComp->mTextAttribute->Color      = mTextAttribute->Color;
    newComp->mTextAttribute->Origin     = mTextAttribute->Origin;
    newComp->mTextAttribute->Scale      = mTextAttribute->Scale;
}

TextRenderer::TextRenderer(Actor* owner, int UpdateOrder)
    : Component(owner, UpdateOrder)
    , mTextAttribute(DBG_NEW TextAttribute)
    , spriteBatch(nullptr)
    , spriteFont(nullptr)
{}

TextRenderer::~TextRenderer()
{
    delete mTextAttribute;
    delete spriteBatch;
    delete spriteFont;
}

void TextRenderer::SaveProperties(std::ofstream& ofs)
{
    Component::SaveProperties(ofs);
    FileIOHelper::SaveFloat     (ofs, ChunkKeys::ROTATION,  mTextAttribute->Rotation);
    FileIOHelper::SaveVector2   (ofs, ChunkKeys::OFFSET,    mTextAttribute->Offset);
    FileIOHelper::SaveVector4   (ofs, ChunkKeys::COLOR,     mTextAttribute->Color);
    FileIOHelper::SaveVector2   (ofs, ChunkKeys::ORIGIN,    mTextAttribute->Origin);
    FileIOHelper::SaveVector2   (ofs, ChunkKeys::SCALE,     mTextAttribute->Scale);
    FileIOHelper::SaveString    (ofs, ChunkKeys::TEXT,      mText);
}

void TextRenderer::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::LoadBasicString   (ifs, mText);
    FileIOHelper::LoadVector2       (ifs, mTextAttribute->Scale);
    FileIOHelper::LoadVector2       (ifs, mTextAttribute->Origin);
    FileIOHelper::LoadVector4       (ifs, mTextAttribute->Color);
    FileIOHelper::LoadVector2       (ifs, mTextAttribute->Offset);
    FileIOHelper::LoadFloat         (ifs, mTextAttribute->Rotation);
    Component::LoadProperties(ifs);
}

void TextRenderer::EditorUIUpdate()
{
    UpdateText();
    UpdateTextAttribute();
}

void TextRenderer::UpdateText()
{
    char str[MAX_STRING_SIZE];
    strcpy_s(str, mText.size()+1, mText.c_str());
    ImGui::InputText(ChunkKeys::TEXT, str, MAX_STRING_SIZE);
    if (mText != str)
        mText = str;
}

void TextRenderer::UpdateTextAttribute()
{
    float rotation  = mTextAttribute->Rotation;
    float offset[2] = { mTextAttribute->Offset.x,mTextAttribute->Offset.y };
    float origin[2] = { mTextAttribute->Origin.x,mTextAttribute->Origin.y };
    float scale[2]  = { mTextAttribute->Scale.x,mTextAttribute->Scale.y };
    float color[4]  = { mTextAttribute->Color.x, mTextAttribute->Color.y, mTextAttribute->Color.z, mTextAttribute->Color.w };

    ImGui::DragFloat    (ChunkKeys::ROTATION, &rotation, FLOATMOD_SPEED);
    ImGui::DragFloat2   (ChunkKeys::OFFSET,   offset,    FLOATMOD_SPEED);
    ImGui::DragFloat2   (ChunkKeys::ORIGIN,   origin,    FLOATMOD_SPEED);
    ImGui::DragFloat2   (ChunkKeys::SCALE,    scale,     FLOATMOD_SPEED);
    ImGui::ColorPicker4 (ChunkKeys::COLOR,    color,     FLOATMOD_SPEED);

    mTextAttribute->Rotation = rotation;
    mTextAttribute->Offset   = DirectX::SimpleMath::Vector2(offset[0], offset[1]);
    mTextAttribute->Origin   = DirectX::SimpleMath::Vector2(origin[0], origin[1]);
    mTextAttribute->Scale    = DirectX::SimpleMath::Vector2(scale[0], scale[1]);
    mTextAttribute->Color    = DirectX::SimpleMath::Vector4(color[0], color[1], color[2], color[3]);
}