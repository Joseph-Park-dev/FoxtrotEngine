// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Animation/FTSpriteAnimation.h"

#include <nlohmann/json.hpp>

#include "FileSystem/FileIOHelper.h"
#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Animation/AnimationFrame.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/Vertex.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Shader/FTGeometryShader.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/FTTexture.h"
#include "Manager/ResourceManager.h"
#include "../Manager/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "Manager/AnimationManager.h"
	#include "ResourceSystem/Shader/FTVertexShader.h"
	#include "EditorUtils.h"
#endif

namespace D3D11
{
	using namespace Common;
	using Microsoft::WRL::ComPtr;
	using namespace Core;
	using namespace Math;
	ResType FTSpriteAnimation::Type = ResType::SPRITE_ANIMATION;

	void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

		FTMeshGroup::SaveProperties(ofs);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTMeshGroup::FRONT_DIR, GetFrontDir());
		FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::FPS, mFPS);
		FileIOHelper::SaveBool(ofs, ChunkKey::FTSpriteAnimation::IS_REPEATED, mIsRepeated);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MAX_FRAME_IDX, mMaxFrameIdx);
		FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MIN_FRAME_IDX, mMinFrameIdx);
		FileIOHelper::SaveVector3(ofs, ChunkKey::FTSpriteAnimation::SIZE_SCALE, GetSizeScale());
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::SPRITE_SHEET, *GetTexture()->GetFileName());
		FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::JSON, *mJSON->GetFileName());

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
	}

	void FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

		Common::FTDS::String jsonKey;
		Common::FTDS::String texKey;
		Math::FTVector3		 sizeScale = Math::FTVector3(1.f, 1.f, 1.f);

		FileIOHelper::LoadBasicString(ifs, jsonKey);
		FileIOHelper::LoadBasicString(ifs, texKey);
		FileIOHelper::LoadVector3(ifs, sizeScale);
		FileIOHelper::LoadInt(ifs, mMinFrameIdx);
		FileIOHelper::LoadInt(ifs, mMaxFrameIdx);
		FileIOHelper::LoadBool(ifs, mIsRepeated);
		FileIOHelper::LoadInt(ifs, mFPS);
		int frontDir = 1;
		FileIOHelper::LoadInt(ifs, frontDir);
		FTMeshGroup::LoadProperties(ifs);

		mJSON = AnimationManager::GetInstance()->GetJSONsFuncGetter()()->GetResource(jsonKey);
		if (!mJSON)
			return;

		SetTexture(D3D11::ResourceManager::GetInstance()->GetSprite(texKey)->GetTexture());
		if (!GetTexture())
			return;

		SetSizeScale(sizeScale);

		0 < frontDir ? SetRightIsFront(true) : SetRightIsFront(false);
	}

	const int FTSpriteAnimation::GetFPS() const
	{
		return mFPS;
	}

	const int FTSpriteAnimation::GetMaxFrameIdx() const { return mMaxFrameIdx; }
	const int FTSpriteAnimation::GetMinFrameIdx() const { return mMinFrameIdx; }

	const size_t FTSpriteAnimation::GetFrameCount() const
	{
		return static_cast<size_t>(mMaxFrameIdx - mMinFrameIdx + 1);
	}

	FTSpriteAnimation::FTSpriteAnimation(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTSprite(resDef, renderer, true)
		, mJSON(nullptr)
		, mMinFrameIdx(0)
		, mMaxFrameIdx(0)
		, mFPS(24)
		, mIsRepeated(true)
	{
		if (!Meshes()->IsEmpty())
			return;

		std::ifstream ifs(resDef.Path);
		this->LoadProperties(ifs);

		if (!mJSON)
			return;

		Initialize(renderer->GetDevice(), renderer->GetContext());
	}

	FTSpriteAnimation::~FTSpriteAnimation()
	{
		// Texture deallocation is handled in FTSprite.
		SetTexture(nullptr);
	}

	void FTSpriteAnimation::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
	{
		// Get sheet size from JSON.
		nlohmann::json sheetSize = mJSON->Data()[SpriteSheetKeys::PROPERTIES][SpriteSheetKeys::SIZE];

		float sheetW = sheetSize.at(SpriteSheetKeys::W);
		float sheetH = sheetSize.at(SpriteSheetKeys::H);

		// Get the number of sprites, create the buffer for the tiles.
		size_t vCount = mMaxFrameIdx - mMinFrameIdx + 1;
		if (GetGCSpriteData())
			delete[] GetGCSpriteData();
		SpriteVertex* vertices = DBG_NEW SpriteVertex[vCount];

		SetGCSpriteData(DBG_NEW SpriteGCData[vCount]);
		// For every sprite data in JSON...
		for (size_t i = mMinFrameIdx; i <= mMaxFrameIdx; ++i)
		{
			// Base array containing sprite data.
			nlohmann::json frame = mJSON->Data()[SpriteSheetKeys::BASE][i];

			// Initialize tile's rect area on sprite sheet.
			float frameX = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::X];
			float frameY = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::Y];

			float mapX = frameX / sheetW;
			float mapY = frameY / sheetH;
			float mapW = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::W] / sheetW;
			float mapH = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::H] / sheetH;

			// Initialize tile's rect area on game screen.
			float screenX = 0.0f;
			float screenY = 0.0f;

			float screenW = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::W];
			float screenH = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::H];

			float pivotX = frame[SpriteSheetKeys::PIVOT][SpriteSheetKeys::X];
			float pivotY = frame[SpriteSheetKeys::PIVOT][SpriteSheetKeys::Y];

			size_t tileIdx			   = i - mMinFrameIdx;
			vertices[tileIdx].Position = Math::FTVector3(screenX, screenY, 0.0f);

			GetGCSpriteData()[tileIdx].Size	 = FTVector2(screenW, screenH);
			GetGCSpriteData()[tileIdx].Scale = FTVector2(1.0f);
			GetGCSpriteData()[tileIdx].Pivot = FTVector2(pivotX, pivotY);
			GetGCSpriteData()[tileIdx].Frame = FTVector4(mapX, mapY, mapW, mapH);
		}

		Mesh* mesh = DBG_NEW Mesh;
		D3D11Utils::CreateVertexBuffer(device, vertices, vCount, mesh->VertexBuffer);
		mesh->VertexCount = static_cast<UINT>(vCount);

		Meshes()->PushBack(mesh);
		delete[] vertices;

		CreateTextureSampler(device);
		InitializeConstantBuffers(device);
	}

#ifdef FOXTROT_EDITOR
	FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimationDef& resDef, D3D11::D3D11Renderer* renderer)
		: FTSprite(resDef, renderer, true)
		, mJSON(resDef.JSON)
		, mMinFrameIdx(resDef.MinFrameIdx)
		, mMaxFrameIdx(resDef.MaxFrameIdx)
		, mFPS(resDef.FPS)
		, mIsRepeated(resDef.IsRepeated)
	{
		Initialize(renderer->GetDevice(), renderer->GetContext());
	}

	void FTSpriteAnimation::UpdateUI()
	{
		GetTexture()->UpdateUI();

		FTVector2 size = GetGCSpriteData()[0].Size;
		Editor::UPDATE_VEC2("Size", size);
		for (size_t i = 0; i < mMaxFrameIdx - mMinFrameIdx + 1; ++i)
			GetGCSpriteData()[i].Size = size;

		Editor::UPDATE_VEC3("Scale size", SizeScale());

		bool val = true;
		0 < GetFrontDir() ? val = true : val = false;

		Editor::UPDATE_BOOL("Is Facing Right", val);
		SetRightIsFront(val);
	}

	void FTSpriteAnimation::AddRefCount()
	{
		mJSON->AddRefCount();
		FTSprite::AddRefCount();
	}

	void FTSpriteAnimation::SubtractRefCount()
	{
		mJSON->SubtractRefCount();
		FTSprite::SubtractRefCount();
	}
#endif
} // namespace D3D11