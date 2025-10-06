// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Animation/FTSpriteAnimation.h"

#include "FileSystem/FileIOHelper.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/Animation/AnimationFrame.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "Managers/AnimationManager.h"
	#include "EditorResourceManager.h"
	#include "ResourceSystem/FTShaders/FTVertexShader.h"
	#include "EditorUtils.h"
#endif

void FTSpriteAnimation::Render(
	int				 meshIndex,
	FoxtrotRenderer* renderer,
	Transform*		 transform,
	Camera*			 camInst,
	FTVertexShader*	 vs,
	FTPixelShader*	 ps,
	FTMaterial*		 mat)
{
	// This enables the resource reusable throughout the Component instances.
	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext(), transform, camInst, mat);

	if (!vs || !ps) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = Meshes()->At(meshIndex);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		context->VSSetConstantBuffers(
			0, 1, GetVCBuf().GetAddressOf());

		if (mSpriteSheet)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(mSpriteSheet->GetSRV().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		context->PSSetShader(ps->GetShader().Get(), 0, 0);

		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		context->IASetInputLayout(vs->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FTMeshGroup::SaveProperties(ofs);
	FileIOHelper::SaveFloat(ofs, ChunkKey::FTSpriteAnimation::FPS, mFPS);
	FileIOHelper::SaveBool(ofs, ChunkKey::FTSpriteAnimation::IS_REPEATED, mIsRepeated);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MAX_FRAME_IDX, mMaxFrameIdx);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MIN_FRAME_IDX, mMinFrameIdx);
	FileIOHelper::SaveVector3(ofs, ChunkKey::FTSpriteAnimation::SIZE_SCALE, GetSizeScale());
	FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::SPRITE_SHEET, mSpriteSheet->GetFileName());
	FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::JSON, mJSON->GetFileName());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
}

void FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FTDS::String jsonKey;
	FTDS::String texKey;
	FTVector3	 sizeScale = FTVector3(1.f, 1.f, 1.f);
	
	FileIOHelper::LoadBasicString(ifs, jsonKey);
	FileIOHelper::LoadBasicString(ifs, texKey);
	FileIOHelper::LoadVector3(ifs, sizeScale);
	FileIOHelper::LoadInt(ifs, mMinFrameIdx);
	FileIOHelper::LoadInt(ifs, mMaxFrameIdx);
	FileIOHelper::LoadBool(ifs, mIsRepeated);
	FileIOHelper::LoadInt(ifs, mFPS);
	FTMeshGroup::LoadProperties(ifs);

	mJSON = ResourceManager::GetInstance()->GetLoadedJSON(jsonKey);
	if (!mJSON)
		return;

	mSpriteSheet = ResourceManager::GetInstance()->GetLoadedTexture(texKey);
	if (!mSpriteSheet)
		return;

	SetSizeScale(sizeScale);
}

const int FTSpriteAnimation::GetFPS() const
{
	return mFPS;
}

const int FTSpriteAnimation::GetMaxFrameIdx() const { return mMaxFrameIdx; }
const int FTSpriteAnimation::GetMinFrameIdx() const { return mMinFrameIdx; }

FTSpriteAnimation::FTSpriteAnimation(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTMeshGroup(resDef, renderer, nullptr)
	, mJSON(nullptr)
	, mSpriteSheet(nullptr)
	, mMinFrameIdx(0)
	, mMaxFrameIdx(0)
	, mFPS(24)
	, mIsRepeated(true)
{
	Process(renderer);
}

FTSpriteAnimation::~FTSpriteAnimation()
{
}

void FTSpriteAnimation::Process(FoxtrotRenderer* renderer)
{
	if (IsProcessed())
		return;

	std::ifstream ifs(GetRelativePath().C_Str());
	this->LoadProperties(ifs);

	if (!mJSON)
		return;

	Initialize(renderer->GetDevice(), renderer->GetContext());

	FTResource::Process();
}

void FTSpriteAnimation::Initialize(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	// Get sheet size from JSON.
	nlohmann::json sheetSize = mJSON->Data()[SpriteSheetKeys::PROPERTIES][SpriteSheetKeys::SIZE];

	float sheetW = sheetSize.at(SpriteSheetKeys::W);
	float sheetH = sheetSize.at(SpriteSheetKeys::H);

	// Get the number of sprites, create the buffer for the tiles.
	size_t tileCount = mJSON->Data()[SpriteSheetKeys::BASE].size();
	Tile* tiles		 = DBG_NEW Tile[tileCount];

	// For every sprite data in JSON...
	for (size_t i = 0; i < tileCount; ++i)
	{
		// Base array containing sprite data.
		nlohmann::json frame = mJSON->Data()[SpriteSheetKeys::BASE][i];

		// Initialize tile's rect area on sprite sheet.
		float mapX = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::X] / sheetW;
		float mapY = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::Y] / sheetH;
		float mapW = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::W] / sheetW;
		float mapH = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::H] / sheetH;

		tiles[i].GetRectOnMap().Set(mapX, mapY, mapW, mapH);

		// Initialize tile's rect area on game screen.
		float screenX = 0.0f;
		float screenY = 0.0f;

		float screenW = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::W];
		float screenH = frame[SpriteSheetKeys::FRAME][SpriteSheetKeys::H];

		// Adjusted size, considering the original W/H ratio of a sprite.
		float adjustedW = 0.f;
		float adjustedH = 0.f;

		adjustedH = 1.0f;
		adjustedW = screenW / screenH;

		//if (screenW <= screenH)
		//{
		//	adjustedW = 1.0f;
		//	adjustedH = screenH / screenW;
		//}
		//else
		//{
		//	
		//}

		tiles[i].GetRectOnScreen().Set(screenX, screenY, adjustedW, adjustedH);
	}

	FTDS::DynamicArray<FTMeshData*> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tiles, mMinFrameIdx, mMaxFrameIdx);
	FTMeshGroup::Initialize(std::move(meshDataBuf), device, context);

	delete[] tiles;
}

#ifdef FOXTROT_EDITOR
FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimationDef& resDef, FoxtrotRenderer* renderer)
	: FTMeshGroup(resDef, renderer, nullptr)
	, mJSON(resDef.JSON)
	, mSpriteSheet(resDef.SpriteSheet)
	, mMinFrameIdx(resDef.MinFrameIdx)
	, mMaxFrameIdx(resDef.MaxFrameIdx)
	, mFPS(resDef.FPS)
	, mIsRepeated(resDef.IsRepeated)
{
	Initialize(renderer->GetDevice(), renderer->GetContext());
}

void FTSpriteAnimation::AddRefCount()
{
	mJSON->AddRefCount();
	mSpriteSheet->AddRefCount();
	FTMeshGroup::AddRefCount();
}

void FTSpriteAnimation::SubtractRefCount()
{
	mJSON->SubtractRefCount();
	mSpriteSheet->SubtractRefCount();
	FTMeshGroup::SubtractRefCount();
}
#endif