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
{}

void AnimatorComponent::Play(bool isRepeated)
{
	if (mCurrentAnim == nullptr)
		printf("ERROR : AnimatorComponent::Play()->Animation is null\n");
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
}

void AnimatorComponent::Play(const UINT key, bool isRepeated)
{
	mCurrentAnim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(key);
	if (mCurrentAnim == nullptr)
		printf("ERROR : AnimatorComponent::Play()->Animation is null\n");
	mCurrentAnim->SetIsFinished(false);
	mCurrentAnim->SetIsRepeated(isRepeated);
	SetMeshGroup(mCurrentAnim);
}

void AnimatorComponent::Stop()
{
	mCurrentAnim->SetIsFinished(true);
}

void AnimatorComponent::CreateAnimationFromTile(std::string&& name, UINT texKey, UINT tileMapKey)
{
	FTSpriteAnimation* animation = DBG_NEW FTSpriteAnimation;
	animation->SetTexKey(texKey);
	animation->SetTileMapKey(tileMapKey);

	FTTileMap* tileMapBuf = ResourceManager::GetInstance()->GetLoadedTileMap(tileMapKey);
	if (tileMapBuf)
		tileMapBuf->ReadCSV();
	if (texKey != VALUE_NOT_ASSIGNED)
		animation->SetTexture(texKey);

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

	//if (mLoadedKeys.size() < 1)
	//{
	//	mCurrentAnim = animation;
	//	//SetMeshGroup(mCurrentAnim);
	//}
	ResourceManager::GetInstance()->LoadResource(
		animation, 
		ResourceManager::GetInstance()->GetSpriteAnimMap()
	);
	printf("FTSpriteAnimation created, %s", name.c_str());
}

void AnimatorComponent::LoadAnimation(const UINT key)
{
	mLoadedKeys.push_back(key);
	if (mLoadedKeys.size() == 1)
	{
		mCurrentAnim = ResourceManager::GetInstance()->GetLoadedSpriteAnim(mLoadedKeys.at(0));
		SetMeshGroup(mCurrentAnim);
	}
}

void AnimatorComponent::LoadProperties(std::ifstream& ifs)
{
	// SpriteRendererComponent::LoadProperties(ifs);
	LogString("Animator LoadProperties() needs to be implemented");
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
		LoadAnimation(key);

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