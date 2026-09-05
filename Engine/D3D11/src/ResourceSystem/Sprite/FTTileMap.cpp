// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Sprite/FTTileMap.h"

#include <fstream>
#include <sstream>
#include <queue>
#include <string>

#include "ResourceSystem/Sprite/Tile.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/BufferSizes.h"
#include "Manager/ResourceManager.h"
#include "ResourceSystem/GenericData/FTCSV.h"
#include "ResourceSystem/D3D11Resource.h"
#include "ResourceSystem/FTResource.h"

#include "FileSystem/DLLPath.h"
#include <../../Core/include/Manager/ResourceManager.h>

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorUtils.h"
	#include "Utility/EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	ResType FTTileMap::Type = ResType::TILEMAP;

	void FTTileMap::Initialize()
	{
		if (!mCSV)
			return;

		// These values cannot be 0;
		assert(mTileWidthOnScreen != 0);
		assert(mTileHeightOnScreen != 0);
		assert(mMaxCountOnMapX != 0);
		assert(mMaxCountOnMapY != 0);

		if (mTileMap)
			delete[] mTileMap;

		size_t column	   = mCSV->GetColumnCount();
		size_t row		   = mCSV->GetRowCount();
		mMaxCountOnScreenX = static_cast<UINT>(column);
		mMaxCountOnScreenY = static_cast<UINT>(row);

		mTileMap = DBG_NEW Tile[column * row];

		for (size_t r = 0; r < row; ++r)
		{
			for (size_t c = 0; c < column; ++c)
			{
				size_t idx = column * r + c;
				if (mCSV->Data().at(idx))
					InitializeTile(mTileMap[idx], c, r, mCSV->Data().at(idx));
			}
		}
#ifdef FOXTROT_EDITOR

		HMODULE coreMod = GetModuleHandleA(Common::DLLPath::CORE);
		assert(coreMod != NULL);
		FARPROC proc = GetProcAddress(coreMod, Core::ProcName::GetCSVs);
		mGetCSVsFunc = reinterpret_cast<GET_CSVS_FUNC>(proc);
#endif // FOXTROT_EDITOR
	}

	void FTTileMap::ReadCSV(Common::FTResourceDef& resDef, Common::FTDS::String& str)
	{
		// These values cannot be 0;
		assert(mTileWidthOnScreen != 0);
		assert(mTileHeightOnScreen != 0);
		assert(mMaxCountOnMapX != 0);
		assert(mMaxCountOnMapY != 0);

		if (mTileMap)
			delete[] mTileMap;

		std::ifstream	myFile;
		std::queue<int> result;

		// Open an existing file
		myFile.open(resDef.Path, std::fstream::in);
		assert(myFile);
		std::string line;
		int			val;
		int			column = 0;
		int			row	   = 0;
		while (std::getline(myFile, line))
		{
			std::stringstream ss(line);
			// Create a stringstream of the current line
			// Extract each integer
			while (ss >> val)
			{
				result.push(val);
				if (ss.peek() == ',')
					ss.ignore();
				++column;
			}
			++row;
		}
		if (row != 0)
			column /= row;
		myFile.close();

		mMaxCountOnScreenX = column;
		mMaxCountOnScreenY = row;

		mTileMap = DBG_NEW Tile[str.GetLength()];
		for (UINT i = 0; i < (UINT)str.GetLength(); ++i)
		{
			UINT idx = str.At(i) - WSTRING_OFFSET;
			InitializeTile(mTileMap[i], i, 0, idx);
		}
	}

	Tile& FTTileMap::GetTile(size_t row, size_t column)
	{
		return mTileMap[mMaxCountOnScreenX * row + column];
	}

	void FTTileMap::SetTiles(Tile* tiles)
	{
		mTileMap = tiles;
	}

	void FTTileMap::SetTileWidth(UINT width)
	{
		mTileWidthOnScreen = width;
	}

	void FTTileMap::SetTileHeight(UINT height)
	{
		mTileHeightOnScreen = height;
	}

	void FTTileMap::SetMaxCountOnScreenX(UINT xCount)
	{
		mMaxCountOnScreenX = xCount;
	}

	void FTTileMap::SetMaxCountOnScreenY(UINT yCount)
	{
		mMaxCountOnScreenY = yCount;
	}

	void FTTileMap::SetMaxCountOnMapX(UINT xCount)
	{
		mMaxCountOnMapX = xCount;
	}

	void FTTileMap::SetMaxCountOnMapY(UINT yCount)
	{
		mMaxCountOnMapY = yCount;
	}

	FTTileMap::FTTileMap(Common::FTResourceDef& resDef)
		: D3D11::D3D11Resource(resDef)
		, mTileWidthOnScreen(0)
		, mTileHeightOnScreen(0)
		, mMaxCountOnMapX(0)
		, mMaxCountOnMapY(0)
		, mTileMap(nullptr)
		, mMaxCountOnScreenX(0)
		, mMaxCountOnScreenY(0)
	{
		if (mTileMap)
			return;

		// This if statement will be triggered only on Editor
		// (When loading all assets from Asset folder)
		std::ifstream ifs(resDef.Path);
		this->LoadProperties(ifs);

		this->Initialize();
	}

	FTTileMap::~FTTileMap()
	{
		if (mTileMap)
		{
			delete[] mTileMap;
			mTileMap = nullptr;
		}
	}

	void FTTileMap::InitializeTile(Tile& tile, size_t column, size_t row, size_t tileNum)
	{
		FTRectArea* rectOnMap = tile.GetRectOnMap();
		// Individual Tile size on tilemap
		float tileWidthOnMap  = 1 / static_cast<float>(mMaxCountOnMapX);
		float tileHeightOnMap = 1 / static_cast<float>(mMaxCountOnMapY);

		size_t tileIndexX = tileNum % static_cast<size_t>(mMaxCountOnMapX);
		size_t tileIndexY = tileNum / static_cast<size_t>(mMaxCountOnMapX);
		rectOnMap->Set(tileWidthOnMap * tileIndexX, tileHeightOnMap * tileIndexY, tileWidthOnMap, tileHeightOnMap);

		FTRectArea* rectOnScreen = tile.GetRectOnScreen();
		rectOnScreen->Set(
			static_cast<float>(column * mTileWidthOnScreen),
			static_cast<float>(row * mTileHeightOnScreen),
			static_cast<float>(mTileWidthOnScreen),
			static_cast<float>(mTileHeightOnScreen));
	}

	void FTTileMap::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);

		FileIOHelper::SaveString(ofs, ChunkKey::FTTileMap::CSV_KEY, *mCSV->GetFileName());
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::SCREEN_WIDTH, mTileWidthOnScreen);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::SCREEN_HEIGHT, mTileHeightOnScreen);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::MAP_MAX_COUNT_X, mMaxCountOnMapX);
		FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTTileMap::MAP_MAX_COUNT_Y, mMaxCountOnMapY);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTileMap::FT_TILEMAP);
	}

	void FTTileMap::LoadProperties(std::ifstream& ifs)
	{
		HMODULE coreMod = GetModuleHandleA(Common::DLLPath::CORE);
		assert(coreMod != NULL);
		FARPROC proc		= GetProcAddress(coreMod, Core::ProcName::GetCSV);
		using GET_CSV_FUNC	= FTCSV* (*)(const char*);
		GET_CSV_FUNC getCSV = reinterpret_cast<GET_CSV_FUNC>(proc);

		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTileMap::FT_TILEMAP);
		FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapY);
		FileIOHelper::LoadUnsignedInt(ifs, mMaxCountOnMapX);
		FileIOHelper::LoadUnsignedInt(ifs, mTileHeightOnScreen);
		FileIOHelper::LoadUnsignedInt(ifs, mTileWidthOnScreen);
		Common::FTDS::String csvKey;
		FileIOHelper::LoadBasicString(ifs, csvKey);
		mCSV = getCSV(csvKey.C_Str());
		// Loading filename
		FileIOHelper::LoadBasicString(ifs, csvKey);
	}

#ifdef FOXTROT_EDITOR
	void FTTileMap::UpdateUI()
	{
		ImVec2 previewSize = ImVec2(100, 100);

		Editor::DisplayResSelection(
			"Select CSV",
			mGetCSVsFunc(),
			mCSV);

		int tileWidthOnScreen  = static_cast<int>(mTileWidthOnScreen);
		int tileHeightOnScreen = static_cast<int>(mTileHeightOnScreen);
		int maxCountOnMapX	   = static_cast<int>(mMaxCountOnMapX);
		int maxCountOnMapY	   = static_cast<int>(mMaxCountOnMapY);

		Editor::UPDATE_INT("Tile width on screen", tileWidthOnScreen);
		Editor::UPDATE_INT("Tile height on screen", tileHeightOnScreen);
		Editor::UPDATE_INT("Max count on Map X", maxCountOnMapX);
		Editor::UPDATE_INT("Max count on Map Y", maxCountOnMapY);

		mTileWidthOnScreen	= static_cast<UINT>(tileWidthOnScreen);
		mTileHeightOnScreen = static_cast<UINT>(tileHeightOnScreen);
		mMaxCountOnMapX		= static_cast<UINT>(maxCountOnMapX);
		mMaxCountOnMapY		= static_cast<UINT>(maxCountOnMapY);
	}

	void FTTileMap::AddRefCount()
	{
		if (mCSV)
			mCSV->AddRefCount();
		FTResource::AddRefCount();
	}

	void FTTileMap::SubtractRefCount()
	{
		if (mCSV)
			mCSV->SubtractRefCount();
		FTResource::SubtractRefCount();
	}
#endif
} // namespace D3D11