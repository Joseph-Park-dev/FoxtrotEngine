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
    , mText(L"Hello text!")
    , mTextAttribute(DBG_NEW TextAttribute)
{
    mTextAttribute->CharSpacing = 100.f;
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
            GetTileMap()->ReadCSV();
            if(GetTexKey() != VALUE_NOT_ASSIGNED)
                SetTexture();
            SetMeshKey(ChunkKeys::PRIMITIVE_SQUARE_BLUE);
            std::vector<MeshData> meshData;
            //GeometryGenerator::MakeTextGrid(meshData, GetTileMap()->GetTiles(), 20);
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
