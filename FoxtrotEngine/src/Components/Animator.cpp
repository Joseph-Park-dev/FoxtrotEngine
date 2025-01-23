// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/Animator.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "ResourceSystem/Tile.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTSpriteAnimation.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Renderer/FoxtrotRenderer.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
#endif // FOXTROT_EDITOR


Animator::Animator(Actor* owner, int updateOrder)
	: TileMapRenderer(owner)
	, mCurrentAnim(nullptr)
{}

Animator::~Animator()
{
	mLoadedKeys.clear();
	for (size_t i = 0; i < mLoadedAnimations.size(); ++i)
	{
		// mCurrentAnim destruction will be taken care of in MeshRenderer
		if (mCurrentAnim != mLoadedAnimations.at(i))
		{
			delete mLoadedAnimations[i];
			mLoadedAnimations[i] = nullptr;
		}
	}
	mLoadedAnimations.clear();
}

void Animator::Play(bool isRepeated)
{
	if (mCurrentAnim == nullptr)
		printf("ERROR : Animator::Play()->Animation is null\n");
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void Animator::Play(const UINT key, bool isRepeated)
{
	mCurrentAnim = mLoadedAnimations.at(key);
	if (mCurrentAnim == nullptr)
		printf("ERROR : Animator::Play()->Animation is null\n");
	SetMeshGroup(mCurrentAnim);
	SetTexKey(mCurrentAnim->GetTexKey());
	SetTexture();
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void Animator::Stop()
{
	mCurrentAnim->SetIsFinished(true);
}

FTSpriteAnimation* Animator::CreateAnimationFromTile(std::string&& name, UINT texKey, UINT tileMapKey)
{
	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;
	if (!GetRenderer())
		printf("ERROR : Animator::CreateAnimationFromTile()-> Renderer is null");
	if (texKey != ChunkKeys::VALUE_NOT_ASSIGNED)
		animation->SetTexKey(texKey);
	if(tileMapKey != ChunkKeys::VALUE_NOT_ASSIGNED)
		animation->SetTileMapKey(tileMapKey);

	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
	if (tileMapBuf)
		tileMapBuf->ReadCSV();

	animation->SetFileName(name);
	animation->SetName(std::move(name));
	animation->SetAnimator(this);
	std::vector<MeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tileMapBuf->GetTiles(),
		tileMapBuf->GetMaxCountOnMapX(),
		tileMapBuf->GetMaxCountOnMapY()
	);
	animation->Initialize(meshDataBuf, GetRenderer()->GetDevice(), GetRenderer()->GetContext());
	printf("FTSpriteAnimation created, %s", name.c_str());
	
	return animation;
}

void Animator::LoadAnimation(const UINT key)
{
	FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
	FTSpriteAnimation* copied = CreateAnimationFromTile(
		std::move(anim->GetName()), 
		anim->GetTexKey(),
		anim->GetTileMapKey()
	);
	mLoadedAnimations.push_back(copied);

	if (mLoadedAnimations.size() == 1)
	{
		delete GetMeshGroup();
		SetMeshGroup(nullptr);
		mCurrentAnim = mLoadedAnimations.at(0);
		SetMeshGroup(mCurrentAnim);

		SetTexKey(mCurrentAnim->GetTexKey());
		SetTexture();
	}
}

void Animator::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::LOADED_KEYS);
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		FileIOHelper::SaveUnsignedInt(ofs, std::to_string(i), mLoadedKeys.at(i));
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::LOADED_KEYS);
}

void Animator::LoadProperties(std::ifstream& ifs)
{
	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::LOADED_KEYS);
	mLoadedKeys.reserve(pack.first);
	for (size_t i = 0; i < pack.first; ++i)
	{
		UINT key = 0;
		FileIOHelper::LoadUnsignedInt(ifs, key);
		mLoadedKeys.push_back(key);
	}
	Component::LoadProperties(ifs);
}

void Animator::Initialize(FTCore* coreInstance)
{
	MeshRenderer::Initialize(coreInstance);
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		LoadAnimation(mLoadedKeys.at(i));
}

void Animator::Update(float deltaTime)
{
	if (mCurrentAnim != nullptr)
		TileMapRenderer::Update(deltaTime);
}

void Animator::LateUpdate(float deltaTime)
{
	if (mCurrentAnim != nullptr)
		mCurrentAnim->Update(deltaTime);
}

void Animator::Render(FoxtrotRenderer* renderer)
{
	if (mCurrentAnim != nullptr)
		mCurrentAnim->Render(renderer, GetTexture());
}

#ifdef FOXTROT_EDITOR
void Animator::EditorUpdate(float deltaTime)
{
	if (mCurrentAnim != nullptr)
	{
		Update(deltaTime);
		mCurrentAnim->Update(deltaTime);
	}
}

void Animator::EditorUIUpdate()
{
	UpdatePlayAnim();
	UpdatePlayList();
	CreateAnimation();
}

void Animator::UpdatePlayAnim()
{
	if (mCurrentAnim)
	{
		if (mCurrentAnim->GetIsFinished())
		{
			if (ImGui::Button("Stop"))
			{
				Stop();
			}
		}
		else
		{
			if (ImGui::Button("Play"))
			{
				Play( true);
			}
		}
	}
}

void Animator::UpdatePlayList()
{
	ImGui::Text("Play List");
	UINT key =
		FTEditorUtils::DisplayResSelection<FTSpriteAnimation>(
			"Load Animation",
			ResourceManager::GetInstance()->GetSpriteAnimMap()
		);
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		mLoadedKeys.push_back(key);
		LoadAnimation(key);
	}

	if (0 < mLoadedKeys.size())
	{
		for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		{
			FTSpriteAnimation* anim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(mLoadedKeys.at(i));
			ImGui::Text(anim->GetName().c_str());
			ImGui::SameLine();
			ImGui::Text(std::to_string(mLoadedKeys.at(i)).c_str());
		}
	}
}

void Animator::CreateAnimation()
{
	if (ImGui::Button("Create Sprite Animation"))
	{
		ImGui::OpenPopup("CreateSpriteAnimation");
	}
	if (ImGui::BeginPopupModal("CreateSpriteAnimation"))
	{
		static char* name = _strdup("NULL");
		ImGui::InputText("Name", name, ChunkKeys::MAX_BUFFER_INPUT_TEXT);

		static UINT texKey = ChunkKeys::VALUE_NOT_ASSIGNED;
		UpdateSprite(texKey);

		static UINT tileMapKey = ChunkKeys::VALUE_NOT_ASSIGNED;
		UpdateCSV(tileMapKey);

		if (ImGui::Button("Create"))
		{
			FTSpriteAnimation* anim = CreateAnimationFromTile(std::string(name), texKey, tileMapKey);
			ResourceManager::GetInstance()->LoadResource(
				anim,
				ResourceManager::GetInstance()->GetSpriteAnimMap()
			);
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::Separator();
		ImGui::EndPopup();
	}
}

void Animator::CloneTo(Actor* actor)
{
	Animator* newComp = DBG_NEW Animator(actor, GetUpdateOrder());
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		newComp->mLoadedKeys.push_back(mLoadedKeys.at(i));
}
#endif // FOXTROT_EDITOR