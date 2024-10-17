#include "FTTileMap.h"

#include <fstream>
#include <sstream>
#include <queue>

#include "ResourceSystem/Tile.h"
#include "Renderer/FTRect.h"
#include "Core/TemplateFunctions.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/ChunkLoader.h"

void FTTileMap::ReadCSV()
{
    // These values cannot be 0;
    assert(mTileWidthOnScreen != 0);
    assert(mTileHeightOnScreen != 0);
    assert(mMaxCountOnMapX != 0);
    assert(mMaxCountOnMapY != 0);

    std::ifstream myFile;
    std::queue<int> result;

    // Open an existing file 
    myFile.open(GetRelativePath(), std::fstream::in);
    assert(myFile);
    std::string line;
    int val;
    int column = 0; int row = 0;
    while (std::getline(myFile, line))
    {
        std::stringstream ss(line);
        // Create a stringstream of the current line
        // Extract each integer
        while (ss >> val)
        {
            result.push(val);
            if (ss.peek() == ',') ss.ignore();
            ++column;
        }
        ++row;
    }
    if (row != 0)
        column /= row;
    myFile.close();

    mMaxCountOnScreenX = column;
    mMaxCountOnScreenY = row;

    mTileMap = new Tile * [row];
    for (int r = 0; r < row; ++r)
    {
        mTileMap[r] = new Tile[column];
        for (int c = 0; c < column; ++c)
        {
            if (!result.empty())
            {
                InitializeTile(&mTileMap[r][c], c, r, result.front());
                result.pop();
            }
        }
    }
}

void FTTileMap::SetTileWidth(UINT width)
{
    mTileWidthOnScreen = width;
}

void FTTileMap::SetTileHeight(UINT height)
{
    mTileHeightOnScreen = height;
}

void FTTileMap::SetMaxCountOnMapX(UINT xCount)
{
    mMaxCountOnMapX = xCount;
}

void FTTileMap::SetMaxCountOnMapY(UINT yCount)
{
    mMaxCountOnMapY = yCount;
}

FTTileMap::FTTileMap()
    : FTResource()
    , mTileWidthOnScreen(0)
    , mTileHeightOnScreen(0)
    , mMaxCountOnMapX(0)
    , mMaxCountOnMapY(0)
{
}

FTTileMap::~FTTileMap()
{
    SafeDeleteArray(mTileMap);
}

void FTTileMap::InitializeTile(Tile* tile, UINT column, UINT row, UINT tileNum)
{
    FTRect* rectOnMap = tile->GetRectOnMap();
    // Individual Tile size on tilemap
    float tileWidthOnMap = 1 / static_cast<float>(mMaxCountOnMapX);
    float tileHeightOnMap = 1 / static_cast<float>(mMaxCountOnMapY);

    int tileIndexX = tileNum % mMaxCountOnMapX;
    int tileIndexY = tileNum / mMaxCountOnMapX;
    rectOnMap->Set(tileWidthOnMap * tileIndexX, tileHeightOnMap * tileIndexY, tileWidthOnMap, tileHeightOnMap);

    FTRect* rectOnScreen = tile->GetRectOnScreen();
    rectOnScreen->Set(column, row, mTileWidthOnScreen, mTileHeightOnScreen);
}

void FTTileMap::SaveProperties(std::ofstream& ofs, UINT key)
{
    FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTILEMAP);
    FTResource::SaveProperties(ofs, key);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_SCREEN_WIDTH, mTileWidthOnScreen);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_SCREEN_HEIGHT, mTileHeightOnScreen);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_MAP_MAX_COUNT_X, mMaxCountOnMapX);
    FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TILEMAP_MAP_MAX_COUNT_Y, mMaxCountOnMapY);
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTILEMAP);

}

UINT FTTileMap::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTILEMAP);
    FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapY);
    FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapX);
    FileIOHelper::LoadUnsignedInt(ifs, mTileHeightOnScreen);
    FileIOHelper::LoadUnsignedInt(ifs, mTileWidthOnScreen);
    UINT key = FTResource::LoadProperties(ifs);
    return key;
}