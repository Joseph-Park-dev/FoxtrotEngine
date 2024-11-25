#include "Components/AnimatorComponent.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"

#include "ResourceSystem/Tile.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTSpriteAnimation.h"
#include "Managers/ResourceManager.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"

AnimatorComponent::AnimatorComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder)
	, mCurrentAnim(nullptr)
	, mIsRepeating(false)
{}

AnimatorComponent::~AnimatorComponent()
{
	std::unordered_map<std::wstring, FTSpriteAnimation*>::iterator iter = mMapAnimation.begin();
	for (; iter != mMapAnimation.end(); ++iter)
	{
		delete iter->second;
	}
	mMapAnimation.clear();
}

void AnimatorComponent::CreateAnimationFromTile(
	const std::string& name, const std::string& fileName
	, int tileSizeX, int tileSizeY, float speed)
{
	//FTSpriteAnimation* animation = FindAnimation(name);
	//if (animation == nullptr)
	//{
	//	//InitializeTileMap(fileName, tileSizeX, tileSizeY);
	//	animation = new FTSpriteAnimation;
	//	animation->SetName(name);
	//	animation->SetAnimator(this);
	//	//for (int y = 0; y < GetTileCountY(); y++)
	//	//	for (int x = 0; x < GetTileCountX(); x++)
	//	//		animation->CreateFrameFromTile(&GetCurrentTileMap()[y][x]);

	//	//mMapAnimation.insert(std::make_pair(name, animation));
	//}
	//else
		//LogString(L"Animation has already been created", name.c_str());
}

void AnimatorComponent::Play(const std::string& name, bool isRepeating)
{
	/*mCurrentAnim = FindAnimation(name);
	if (mCurrentAnim == nullptr)
	{
		LogString("Animation is null");
	}
	mIsRepeating = isRepeating;*/
}

void AnimatorComponent::LoadProperties(std::ifstream& ifs)
{
	// SpriteRendererComponent::LoadProperties(ifs);
	LogString("Animator LoadProperties() needs to be implemented");
}

FTSpriteAnimation* AnimatorComponent::FindAnimation(const std::wstring& name)
{
	std::unordered_map<std::wstring, FTSpriteAnimation*>::iterator iter = mMapAnimation.find(name);
	if (iter == mMapAnimation.end())
	{
		LogString(L"Cannot find animation", name.c_str());
		return nullptr;
	}
	else
	{
		if(iter->second)
			return iter->second;
		else
		{
			LogString(L"Animation is null", name.c_str());
			return nullptr;
		}
	}
}

void AnimatorComponent::LateUpdate(float deltaTime)
{
	if (mCurrentAnim != nullptr)
	{
		mCurrentAnim->Update(deltaTime);
		if (mCurrentAnim->IsFinished())
		{
			if (mIsRepeating)
				mCurrentAnim->SetFrame(0);
		}
	}
}

void AnimatorComponent::Render(FoxtrotRenderer* renderer)
{
	if (mCurrentAnim != nullptr)
	{
		if (!mCurrentAnim->IsFinished())
		{
			FTVector3 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
			// DrawIndividualTileOnPosEX(renderer, worldPos, mCurrentAnim->GetTile());
		}
	}
}

#ifdef FOXTROT_EDITOR
void AnimatorComponent::CloneTo(Actor* actor)
{
	CLONE_TO_NOT_IMPLEMENTED
}
#endif // FOXTROT_EDITOR