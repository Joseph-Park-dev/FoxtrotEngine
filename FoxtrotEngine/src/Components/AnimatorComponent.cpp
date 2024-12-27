#include "Components/AnimatorComponent.h"

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

AnimatorComponent::AnimatorComponent(Actor* owner, int drawOrder, int updateOrder)
	: TileMapComponent(owner, drawOrder)
	, mCurrentAnim(nullptr)
{}

AnimatorComponent::~AnimatorComponent()
{
	mLoadedKeys.clear();
	for (size_t i = 0; i < mLoadedAnimations.size(); ++i)
	{
		// mCurrentAnim destruction will be taken care of in MeshRendererComponent
		if (mCurrentAnim != mLoadedAnimations.at(i))
		{
			delete mLoadedAnimations[i];
			mLoadedAnimations[i] = nullptr;
		}
	}
	mLoadedAnimations.clear();
}

void AnimatorComponent::Play(bool isRepeated)
{
	if (mCurrentAnim == nullptr)
		printf("ERROR : AnimatorComponent::Play()->Animation is null\n");
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void AnimatorComponent::Play(const UINT key, bool isRepeated)
{
	mCurrentAnim = mLoadedAnimations.at(key);
	if (mCurrentAnim == nullptr)
		printf("ERROR : AnimatorComponent::Play()->Animation is null\n");
	SetMeshGroup(mCurrentAnim);
	SetTexKey(mCurrentAnim->GetTexKey());
	SetTexture();
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void AnimatorComponent::Stop()
{
	mCurrentAnim->SetIsFinished(true);
}

FTSpriteAnimation* AnimatorComponent::CreateAnimationFromTile(std::string&& name, UINT texKey, UINT tileMapKey)
{
	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;
	if (!GetRenderer())
		printf("ERROR : AnimatorComponent::CreateAnimationFromTile()-> Renderer is null");
	if (texKey != VALUE_NOT_ASSIGNED)
		animation->SetTexKey(texKey);
	if(tileMapKey != VALUE_NOT_ASSIGNED)
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

void AnimatorComponent::LoadAnimation(const UINT key)
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

void AnimatorComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::LOADED_KEYS);
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		FileIOHelper::SaveUnsignedInt(ofs, std::to_string(i), mLoadedKeys.at(i));
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::LOADED_KEYS);
}

void AnimatorComponent::LoadProperties(std::ifstream& ifs)
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

void AnimatorComponent::Initialize(FTCore* coreInstance)
{
	MeshRendererComponent::Initialize(coreInstance);
	for (size_t i = 0; i < mLoadedKeys.size(); ++i)
		LoadAnimation(mLoadedKeys.at(i));
}

void AnimatorComponent::Update(float deltaTime)
{
	if (mCurrentAnim != nullptr)
		TileMapComponent::Update(deltaTime);
}

void AnimatorComponent::LateUpdate(float deltaTime)
{
	if (mCurrentAnim != nullptr)
		mCurrentAnim->Update(deltaTime);
}

void AnimatorComponent::Render(FoxtrotRenderer* renderer)
{
	if (mCurrentAnim != nullptr)
		mCurrentAnim->Render(renderer, GetTexture());
}

#ifdef FOXTROT_EDITOR
void AnimatorComponent::EditorUpdate(float deltaTime)
{
	if (mCurrentAnim != nullptr)
	{
		Update(deltaTime);
		mCurrentAnim->Update(deltaTime);
	}
}

void AnimatorComponent::EditorUIUpdate()
{
	UpdatePlayAnim();
	UpdatePlayList();
	CreateAnimation();
}

void AnimatorComponent::UpdatePlayAnim()
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

void AnimatorComponent::UpdatePlayList()
{
	ImGui::Text("Play List");
	UINT key = ResourceManager::GetInstance()->ShowResourceSelection(
		"Load Animation",
		ResourceManager::GetInstance()->GetSpriteAnimMap()
	);
	if (key != VALUE_NOT_ASSIGNED)
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

void AnimatorComponent::CreateAnimation()
{
	if (ImGui::Button("Create Sprite Animation"))
	{
		ImGui::OpenPopup("CreateSpriteAnimation");
	}
	if (ImGui::BeginPopupModal("CreateSpriteAnimation"))
	{
		static char* name = _strdup("NULL");
		ImGui::InputText("Name", name, ChunkKeys::MAX_BUFFER_INPUT_TEXT);

		static UINT texKey = VALUE_NOT_ASSIGNED;
		UpdateSprite(texKey);

		static UINT tileMapKey = VALUE_NOT_ASSIGNED;
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

void AnimatorComponent::OnConfirmUpdate()
{
}

void AnimatorComponent::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}
#endif // FOXTROT_EDITOR