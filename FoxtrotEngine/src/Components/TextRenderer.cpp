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
    TextRenderer* newComp = DBG_NEW TextRenderer(actor, GetDrawOrder(), GetUpdateOrder());
    newComp->SetMeshKey(this->GetMeshKey());
    newComp->SetTexKey(this->GetTexKey());
    newComp->SetTileMapKey(this->GetTileMapKey());
}

TextRenderer::TextRenderer(Actor* owner, int drawOrder, int UpdateOrder)
    : TileMapRenderer(owner, drawOrder, UpdateOrder)
    , mTextAttribute(DBG_NEW TextAttribute)
{
    mTextAttribute->CharSpacing = 100.f;
    mText.append("New Text");
}

TextRenderer::~TextRenderer()
{
    delete mTextAttribute;
}

void TextRenderer::InitializeTileMap()
{
    if (GetTileMapKey() != VALUE_NOT_ASSIGNED)
    {
        SetTileMap(ResourceManager::GetInstance()->GetLoadedTileMap(GetTileMapKey()));
        if (GetTileMap())
        {
            GetTileMap()->ReadCSV(mText);
            if(GetTexKey() != VALUE_NOT_ASSIGNED)
                SetTexture();
            SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
            std::vector<MeshData> meshData;
            GeometryGenerator::MakeSpriteTextGrid(meshData, GetTileMap()->GetTiles(), mText.size());
            MeshRenderer::InitializeMesh(meshData);
        }
    }
}

void TextRenderer::SaveProperties(std::ofstream& ofs)
{
    TileMapRenderer::SaveProperties(ofs);
}

void TextRenderer::LoadProperties(std::ifstream& ifs)
{
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
