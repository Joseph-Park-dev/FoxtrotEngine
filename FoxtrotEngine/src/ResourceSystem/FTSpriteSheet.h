#pragma once
#include "ResourceSystem/FTResource.h"

#include <nlohmann/json.hpp>

class Tile;
class FTJSON;
class FTVector2;

class FTSpriteSheet :
	public FTResource
{
public:
	void Initialize();

public:
	Tile*	GetTiles();
	size_t& GetTilesCount();

public:
	FTSpriteSheet();
	~FTSpriteSheet() override;

protected:
	void InitializeTile(Tile& tile, nlohmann::json& json);

private:
	FTDS::String mJSONKey;
	Tile*		 mTiles;
	size_t		 mTilesCount;

	FTVector2 mSheetSize;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

private:
	void InitializeProperties(nlohmann::json& json);
	void InitializeRectOnMap(Tile& tile, float posOnMapX, float posOnMapY, float widthOnMap, float heightOnMap);
	void InitializeRectOnScreen(Tile& tile, float posOnMapX, float posOnMapY, float widthOnMap, float heightOnMap);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override;

	virtual void AddRefCount() override;
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	namespace SpriteSheet
	{
		constexpr const char* JSON_KEY	   = "JSON Key";
		constexpr const char* SPRITE_SHEET = "Sprite Sheet";
		constexpr const char* SHEET_SIZE   = "Sheet Size";
	} // namespace SpriteSheet
} // namespace ChunkKey

namespace SpriteSheetKeys
{
	constexpr const char* BASE		 = "frames";
	constexpr const char* PROPERTIES = "meta";
	constexpr const char* SIZE		 = "size";
	constexpr const char* FRAME		 = "frame";
	constexpr const char* ROTATED	 = "rotated";
	constexpr const char* TRIMMED	 = "trimmed";

	constexpr const char* X = "x";
	constexpr const char* Y = "y";
	constexpr const char* W = "w";
	constexpr const char* H = "h";

} // namespace SpriteSheetKeys