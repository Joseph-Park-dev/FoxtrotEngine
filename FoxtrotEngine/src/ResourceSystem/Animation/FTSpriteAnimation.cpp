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
#include "Renderer/Camera.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/Animation/AnimationFrame.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTShaders/FTGeometryShader.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "Managers/AnimationManager.h"
	#include "EditorResourceManager.h"
	#include "ResourceSystem/FTShaders/FTVertexShader.h"
	#include "EditorUtils.h"
#endif

void FTSpriteAnimation::Render(
	int				  meshIndex,
	FoxtrotRenderer*  renderer,
	Transform*		  transform,
	Camera*			  camInst,
	FTVertexShader*	  vs,
	FTGeometryShader* gs,
	FTPixelShader*	  ps,
	FTMaterial*		  mat)
{
	// This enables the resource reusable throughout the Component instances.
	UpdateConstantBuffers(meshIndex, renderer->GetDevice(), renderer->GetContext(), transform, camInst, mat, GetFrontDir());

	if (!vs || !ps || !gs || !mat) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(SpriteVertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = Meshes()->At(0);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		if (GetTexture())
		{
			ID3D11ShaderResourceView* const resViews[] = {
				GetTexture()->GetSRV().Get()
			};
			context->PSSetShaderResources(0, 1, resViews);
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);
		context->VSSetConstantBuffers(
			0, 1, GetVCBuf().GetAddressOf());

		context->GSSetShader(gs->GetShader().Get(), 0, 0);

		ID3D11Buffer* const gsCBuffers[] = {
			GetGCMatBuf().Get(),
			GetGCSpriteBuf().Get(),
		};
		context->GSSetConstantBuffers(0, 2, gsCBuffers);

		context->PSSetShader(ps->GetShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		context->IASetInputLayout(vs->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->Draw(1, 0);
	}
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FTMeshGroup::SaveProperties(ofs);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTMeshGroup::FRONT_DIR, GetFrontDir());
	FileIOHelper::SaveFloat(ofs, ChunkKey::FTSpriteAnimation::FPS, mFPS);
	FileIOHelper::SaveBool(ofs, ChunkKey::FTSpriteAnimation::IS_REPEATED, mIsRepeated);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MAX_FRAME_IDX, mMaxFrameIdx);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MIN_FRAME_IDX, mMinFrameIdx);
	FileIOHelper::SaveVector3(ofs, ChunkKey::FTSpriteAnimation::SIZE_SCALE, GetSizeScale());
	FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::SPRITE_SHEET, GetTexture()->GetFileName());
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
	int frontDir = 1;
	FileIOHelper::LoadInt(ifs, frontDir);
	FTMeshGroup::LoadProperties(ifs);

	mJSON = ResourceManager::GetInstance()->GetLoadedJSON(jsonKey);
	if (!mJSON)
		return;

	SetTexture(ResourceManager::GetInstance()->GetLoadedTexture(texKey));
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

FTSpriteAnimation::FTSpriteAnimation(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTSprite(resDef, renderer, true)
	, mJSON(nullptr)
	, mMinFrameIdx(0)
	, mMaxFrameIdx(0)
	, mFPS(24)
	, mIsRepeated(true)
{
	Process(renderer);
}

FTSpriteAnimation::~FTSpriteAnimation()
{
	// Texture deallocation is handled in FTSprite.
	SetTexture(nullptr);
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

void FTSpriteAnimation::UpdateConstantBuffers(int meshIndex, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, Transform* transform, Camera* camInst, FTMaterial* mat, const int frontDir)
{
	size_t gcDataCount = mMaxFrameIdx - mMinFrameIdx + 1;
	FTSprite::UpdateConstantBuffers(device, context, transform, camInst, mat, frontDir, gcDataCount);
	D3D11Utils::UpdateBuffer(context, GetGCSpriteData()[meshIndex], GetGCSpriteBuf());

	if (mat)
		mat->UpdateBuffer(context);
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
		vertices[tileIdx].Position = Vector3(screenX, screenY, 0.0f);

		GetGCSpriteData()[tileIdx].Size	 = Vector2(screenW, screenH);
		GetGCSpriteData()[tileIdx].Scale = Vector2(1.0f);
		GetGCSpriteData()[tileIdx].Pivot = Vector2(pivotX, pivotY);
		GetGCSpriteData()[tileIdx].Frame = Vector4(mapX, mapY, mapW, mapH);
	}

	Mesh* mesh = DBG_NEW Mesh;
	D3D11Utils::CreateVertexBuffer(device, vertices, vCount, mesh->VertexBuffer);
	mesh->VertexCount = vCount;

	Meshes()->PushBack(mesh);
	delete[] vertices;

	CreateTextureSampler(device);
	InitializeConstantBuffers(device);
}

#ifdef FOXTROT_EDITOR
FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimationDef& resDef, FoxtrotRenderer* renderer)
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

	Vector2 size = GetGCSpriteData()[0].Size;
	CommandHistory::GetInstance()->UpdateVector2Value("Size", size);
	for (size_t i = 0; i < mMaxFrameIdx - mMinFrameIdx + 1; ++i)
		GetGCSpriteData()[i].Size = size;

	FTVector3 scale = GetSizeScale();
	CommandHistory::GetInstance()->UpdateVector3Value("Scale size", scale);
	SetSizeScale(scale);

	bool val = true;
	0 < GetFrontDir() ? val = true : val = false;

	CommandHistory::GetInstance()->UpdateBoolValue("Is Facing Right", val);
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