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
#include "Renderer/FoxtrotRenderer.h"

AnimatorComponent::AnimatorComponent(Actor* owner, int drawOrder, int updateOrder)
	: TileMapComponent(owner, drawOrder)
	, mCurrentAnim(nullptr)
{}

AnimatorComponent::~AnimatorComponent()
{
	std::unordered_map<std::string, FTSpriteAnimation*>::iterator iter = mLoadedAnimations.begin();
	for (; iter != mLoadedAnimations.end(); ++iter)
	{
		if (iter->second)
		{
			if (iter->second == mCurrentAnim)
			{
				mCurrentAnim = nullptr;
			}
			else
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}
	}
	mLoadedAnimations.clear();
}

void AnimatorComponent::Play(const std::string& name, bool isRepeated)
{
	mCurrentAnim = FindAnimation(name);
	if (mCurrentAnim == nullptr)
		printf("ERROR : AnimatorComponent::Play()->Animation is null\n");
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void AnimatorComponent::Stop()
{
	mCurrentAnim->SetIsFinished(true);
}

void AnimatorComponent::CreateAnimationFromTile(std::string&& name, UINT texKey, UINT tileMapKey)
{
	FTSpriteAnimation* animation = FindAnimation(name);
	if (animation == nullptr)
	{
		animation = DBG_NEW FTSpriteAnimation;
		animation->SetTexKey(texKey);
		animation->SetTileMapKey(tileMapKey);

		FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
		if (tileMapBuf)
			tileMapBuf->ReadCSV();
		if (texKey != VALUE_NOT_ASSIGNED)
			animation->SetTexture(texKey);

		animation->SetName(std::move(name));
		animation->SetAnimator(this);
		std::vector<MeshData> meshDataBuf;
		GeometryGenerator::MakeSpriteAnimation(
			meshDataBuf, tileMapBuf->GetTiles(),
			tileMapBuf->GetMaxCountOnMapX(),
			tileMapBuf->GetMaxCountOnMapY()
		);
		animation->Initialize(meshDataBuf, GetRenderer()->GetDevice(), GetRenderer()->GetContext());
		if (mLoadedAnimations.size() < 1)
		{
			mCurrentAnim = animation;
			SetMeshGroup(mCurrentAnim);
		}

		mLoadedAnimations.insert(std::make_pair(name, animation));
		printf("FTSpriteAnimation created, %s", name.c_str());
	}
	else
		printf("AnimatorComponent::CreateAnimationFromTile()->Animation has already been created; %s\n", name.c_str());
}

void AnimatorComponent::LoadProperties(std::ifstream& ifs)
{
	// SpriteRendererComponent::LoadProperties(ifs);
	LogString("Animator LoadProperties() needs to be implemented");
}

FTSpriteAnimation* AnimatorComponent::FindAnimation(const std::string& name)
{
	std::unordered_map<std::string, FTSpriteAnimation*>::iterator iter = mLoadedAnimations.find(name);
	if (iter == mLoadedAnimations.end())
	{
		printf("ERROR : AnimatorComponent::FindAnimation()-> Cannot find animation %s\n", name.c_str());
		return nullptr;
	}
	else
	{
		if(iter->second)
			return iter->second;
		else
		{
			printf("ERROR : AnimatorComponent::FindAnimation()-> Animation is null\n");
			return nullptr;
		}
	}
}

void AnimatorComponent::Initialize(FTCore* coreInstance)
{
	TileMapComponent::Initialize(coreInstance);
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
		mCurrentAnim->Render(renderer);
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
	std::unordered_map<std::string, FTSpriteAnimation*>::iterator iter = mLoadedAnimations.begin();
	for (; iter != mLoadedAnimations.end(); ++iter)
	{
		UINT texKey = (*iter).second->GetTexKey();
		UpdateSprite(texKey);

		UINT tileMapKey = (*iter).second->GetTileMapKey();
		UpdateCSV();
		(*iter).second->UpdateUI();

		(*iter).second->SetTexKey(texKey);
		(*iter).second->SetTileMapKey(texKey);

		ImGui::Separator();
		UpdatePlayAnim();
	}
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
				Play(mCurrentAnim->GetName(), true);
			}
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
			CreateAnimationFromTile(std::string(name), texKey, tileMapKey);
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