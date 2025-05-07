#include "FTSpriteSheet.h"

#include "Math/FTMath.h"

#include "Renderer/FTRectArea.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "FileSystem/NullKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR


void FTSpriteSheet::Initialize()
{
	if (mJSONKey == ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		return;

#ifdef FOXTROT_EDITOR
	FTJSON* json = EditorResourceManager::GetInstance()->GetLoadedJSON(mJSONKey);
#else
	FTJSON* json = ResourceManager::GetInstance()->GetLoadedJSON(mJSONKey);
#endif // FOXTROT_EDITOR

	InitializeProperties(json->Data().at(SpriteSheetKeys::PROPERTIES));

	size_t tileCount = json->Data().at(SpriteSheetKeys::BASE).size();
	mTiles = DBG_NEW Tile[tileCount];

	size_t i = 0;
	for (auto& item : json->Data().at(SpriteSheetKeys::BASE))
	{
		if (item.is_null())
			return;
		InitializeTile(mTiles[i], item);
		++i;
	}
}

Tile* FTSpriteSheet::GetTiles()
{
	return mTiles;
}

FTSpriteSheet::FTSpriteSheet()
	: mJSONKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mTiles(nullptr)
	, mSheetSize(FTVector2::Zero)
{
}

FTSpriteSheet::~FTSpriteSheet()
{
	delete[] mTiles;
}

void FTSpriteSheet::InitializeTile(Tile& tile, nlohmann::json& json)
{
	float posOnMapX = json[SpriteSheetKeys::FRAME][SpriteSheetKeys::X] / mSheetSize.x;
	float posOnMapY = json[SpriteSheetKeys::FRAME][SpriteSheetKeys::Y] / mSheetSize.y;
	float width		= json[SpriteSheetKeys::FRAME][SpriteSheetKeys::W] / mSheetSize.x;
	float height	= json[SpriteSheetKeys::FRAME][SpriteSheetKeys::H] / mSheetSize.y;

	InitializeRectOnMap(tile, posOnMapX, posOnMapY, width, height);

	float posOnScreenX = 0.0f;
	float posOnScreenY = 0.0f;

	float srcWidth = json[SpriteSheetKeys::FRAME][SpriteSheetKeys::W];
	float srcHeight = json[SpriteSheetKeys::FRAME][SpriteSheetKeys::H];

	float heightOnScreen = 1.0f;
	float widthOnScreen = srcWidth / srcHeight;

	InitializeRectOnScreen(tile, posOnScreenX, posOnScreenY, widthOnScreen, heightOnScreen);
}

void FTSpriteSheet::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SpriteSheet::SPRITE_SHEET);
	FTResource::SaveProperties(ofs, key);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::SpriteSheet::JSON_KEY, mJSONKey);
	FileIOHelper::SaveVector2(ofs, ChunkKey::SpriteSheet::SHEET_SIZE, mSheetSize);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SpriteSheet::SPRITE_SHEET);
}

UINT FTSpriteSheet::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SpriteSheet::SPRITE_SHEET);
	FileIOHelper::LoadVector2(ifs, mSheetSize);
	FileIOHelper::LoadUnsignedInt(ifs, mJSONKey);
	return FTResource::LoadProperties(ifs);
}

void FTSpriteSheet::InitializeProperties(nlohmann::json& json)
{
	float width	 = json.at(SpriteSheetKeys::SIZE).at(SpriteSheetKeys::W);
	float height = json.at(SpriteSheetKeys::SIZE).at(SpriteSheetKeys::H);
	mSheetSize	 = FTVector2(width, height);
}

void FTSpriteSheet::InitializeRectOnMap(Tile& tile, float posX, float posY, float width, float height)
{
	FTRectArea& rectOnMap = tile.GetRectOnMap();
	tile.GetRectOnMap().Set(posX, posY, width, height);
}

void FTSpriteSheet::InitializeRectOnScreen(Tile& tile, float posX, float posY, float width, float height)
{
	FTRectArea& rectOnScreen = tile.GetRectOnScreen();
	tile.GetRectOnScreen().Set(posX, posY, width, height);
}

#ifdef FOXTROT_EDITOR
void FTSpriteSheet::UpdateUI()
{
	ImVec2 previewSize = ImVec2(100, 100);

	CommandHistory::GetInstance()->UpdateStringValue("Sheet Name", FileName());

	FTEditorUtils::DisplayResSelection(
		"Select JSON",
		EditorResourceManager::GetInstance()->GetMapJSONs(),
		mJSONKey);

	std::string text = { "Sheet size : " };
	text += std::to_string(mSheetSize.x) + " ,";
	text += std::to_string(mSheetSize.y);
	ImGui::Text(text.c_str());
}
#endif // FOXTROT_EDITOR