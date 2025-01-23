#include "Components/TextRenderer.h"

#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "ResourceSystem/GeometryGenerator.h"

std::string TextRenderer::GetName() const
{
	return "TextRenderer";
}

void TextRenderer::Initialize(FTCore* coreInstance)
{
    MeshRenderer::Initialize(coreInstance);
    this->InitializeTileMap();
}

void TextRenderer::Render(FoxtrotRenderer* renderer)
{
    if (GetMeshGroup())
    {
        for (size_t i = 0; i < mText.size(); ++i)
        {
            GetMeshGroup()->Render(renderer, GetTexture());
        }
    }
}

void TextRenderer::CloneTo(Actor* actor)
{
    TextRenderer* newComp = DBG_NEW TextRenderer(actor, GetUpdateOrder());
    newComp->SetMeshKey(this->GetMeshKey());
    newComp->SetTexKey(this->GetTexKey());
    newComp->SetTileMapKey(this->GetTileMapKey());
    newComp->mText = this->mText;

    newComp->mTextAttribute->Scale          = mTextAttribute->Scale;
    newComp->mTextAttribute->CharSpacing    = mTextAttribute->CharSpacing;
    newComp->mTextAttribute->LineSpacing    = mTextAttribute->LineSpacing;
    newComp->mTextAttribute->MaxChars       = mTextAttribute->MaxChars;
}

TextRenderer::TextRenderer(Actor* owner, int UpdateOrder)
    : TileMapRenderer(owner, UpdateOrder)
    , mTextAttribute(DBG_NEW TextAttribute)
{}

TextRenderer::~TextRenderer()
{
    delete mTextAttribute;
}

void TextRenderer::InitializeTileMap()
{
    if (GetTileMapKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
    {
        SetTileMap(ResourceManager::GetInstance()->GetLoadedTileMap(GetTileMapKey()));
        if (GetTileMap())
        {
            GetTileMap()->ReadCSV(mText);
            if(GetTexKey() != ChunkKeys::VALUE_NOT_ASSIGNED)
                SetTexture();
            SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
            std::vector<MeshData> meshData;
            GeometryGenerator::MakeSpriteTextGrid(meshData, GetTileMap()->GetTiles(), mText.size(), mTextAttribute);
            MeshRenderer::InitializeMesh(meshData);
        }
    }
}

void TextRenderer::SaveProperties(std::ofstream& ofs)
{
    TileMapRenderer::SaveProperties(ofs);
    FileIOHelper::SaveFloat(ofs, ChunkKeys::SCALE, mTextAttribute->Scale);
    FileIOHelper::SaveFloat(ofs, ChunkKeys::CHAR_SPACING, mTextAttribute->CharSpacing);
    FileIOHelper::SaveFloat(ofs, ChunkKeys::LINE_SPACING, mTextAttribute->LineSpacing);
    FileIOHelper::SaveInt(ofs, ChunkKeys::MAX_CHAR_PER_LINE, mTextAttribute->MaxChars);
    FileIOHelper::SaveString(ofs, ChunkKeys::TEXT, mText);
}

void TextRenderer::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::LoadBasicString(ifs, mText);
    FileIOHelper::LoadInt   (ifs, mTextAttribute->MaxChars);
    FileIOHelper::LoadFloat (ifs, mTextAttribute->LineSpacing);
    FileIOHelper::LoadFloat (ifs, mTextAttribute->CharSpacing);
    FileIOHelper::LoadFloat (ifs, mTextAttribute->Scale);

    UINT texKey = 0;
    FileIOHelper::LoadUnsignedInt(ifs, texKey);
    SetTexKey(texKey);

    UINT tileMapKey = 0;
    FileIOHelper::LoadUnsignedInt(ifs, tileMapKey);
    SetTileMapKey(tileMapKey);
    Component::LoadProperties(ifs);
}

void TextRenderer::EditorUIUpdate()
{
    CHECK_RENDERER(GetRenderer());
    TileMapRenderer::UpdateSprite();
    TileMapRenderer::UpdateCSV();
    UpdateText();
    UpdateTextAttribute();
    OnConfirmUpdate();
}

void TextRenderer::OnConfirmUpdate()
{
    if (ImGui::Button("Update"))
        this->InitializeTileMap();
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
    ImGui::DragFloat(ChunkKeys::SCALE, &mTextAttribute->Scale, FLOATMOD_SPEED);
    ImGui::DragFloat(ChunkKeys::CHAR_SPACING, &mTextAttribute->CharSpacing, FLOATMOD_SPEED);
    ImGui::DragFloat(ChunkKeys::LINE_SPACING, &mTextAttribute->LineSpacing, FLOATMOD_SPEED);
    ImGui::DragInt(ChunkKeys::MAX_CHAR_PER_LINE, &mTextAttribute->MaxChars);
}
