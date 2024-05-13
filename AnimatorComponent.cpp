#include "AnimatorComponent.h"
#include "Actor.h"
#include "Camera2D.h"
#include "Tile.h"
#include "FTTexture.h"
#include "ResourceManager.h"
#include "TemplateFunctions.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "FTCore.h"

AnimatorComponent::AnimatorComponent(Actor* owner, int drawOrder, int updateOrder)
	: TileMapComponent(owner, drawOrder)
	, mCurrentAnim(nullptr)
	, mIsRepeating(false)
{}

AnimatorComponent::~AnimatorComponent()
{
	std::unordered_map<std::wstring, Animation*>::iterator iter = mMapAnimation.begin();
	for (; iter != mMapAnimation.end(); ++iter)
	{
		delete iter->second;
	}
	mMapAnimation.clear();
}

void AnimatorComponent::CreateAnimationFromTile(
	const std::wstring& name, const std::wstring& fileName
	, int tileSizeX, int tileSizeY, float speed)
{
	Animation* animation = FindAnimation(name);
	if (animation == nullptr)
	{
		InitializeTileMap(fileName, tileSizeX, tileSizeY);
		animation = new Animation;
		animation->SetName(name);
		animation->SetAnimator(this);
		for (int y = 0; y < GetTileCountY(); y++)
			for (int x = 0; x < GetTileCountX(); x++)
				animation->CreateFrameFromTile(&GetCurrentTileMap()[y][x]);

		mMapAnimation.insert(std::make_pair(name, animation));
	}
	else
		LogString(L"Animation has already been created", name.c_str());
}

void AnimatorComponent::Play(const std::wstring& name, bool isRepeating)
{
	mCurrentAnim = FindAnimation(name);
	if (mCurrentAnim == nullptr)
	{
		LogString("Animation is null");
	}
	mIsRepeating = isRepeating;
}

Animation* AnimatorComponent::FindAnimation(const std::wstring& name)
{
	std::unordered_map<std::wstring, Animation*>::iterator iter = mMapAnimation.find(name);
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

void AnimatorComponent::SaveProperties(std::ofstream& ofs)
{
	SpriteComponent::SaveProperties(ofs);
	for (const std::pair<std::wstring, Animation*>& pair : mMapAnimation)
		FileIOHelper::AddWString(ofs, pair.first);
}

void AnimatorComponent::LoadProperties(std::ifstream& ifs)
{
	SpriteComponent::LoadProperties(ifs);
	LogString("Animator LoadProperties() needs to be implemented");
}

void AnimatorComponent::SaveAnimation(const std::wstring& animName, const std::wstring& path)
{
	FILE* file = nullptr;
	_wfopen_s(&file, path.c_str(), L"wb");
	assert(file);

	fprintf(file, "[Animation Name]\n");
	// This ensures the Animation with animName has been created.
	std::string strName = std::string(animName.begin(), animName.end());
	fprintf(file, strName.c_str());
	fprintf(file, "\n");

	fprintf(file, "[Frame Count]\n");
	// This ensures the Animation with animName has been created.
	size_t frameCount = mMapAnimation[animName]->GetReel().size();
	fprintf(file, "%d\n", frameCount);

	fprintf(file, "[Frame Data]\n");
	std::vector<AnimationFrame> mReel = mMapAnimation[animName]->GetReel();
	for (size_t i = 0; i < mReel.size(); ++i)
	{
		fprintf(file, "[Frame Index]\n");
		fprintf(file, "%d\n", i);
		fprintf(file, "[Tile Width]\n");
		fprintf(file, "%d\n", mReel[i].tile.GetWidth());
		fprintf(file, "[Tile Height]\n");
		fprintf(file, "%d\n", mReel[i].tile.GetHeight());
		fprintf(file, "[Tile Index]\n");
		fprintf(file, "%d\n", mReel[i].tile.GetTileNum());
		fprintf(file, "[Frame Duration]\n");
		fprintf(file, "%f\n", mReel[i].duration);
	}
	fprintf(file, "[FTTexture Name]\n");
	// This ensures the Animation with animName has been created.
	//std::wstring strKey = GetTexture()->GetKey();
	//strName = std::string(strKey.begin(), strKey.end());
	fprintf(file, strName.c_str());
	fprintf(file, "\n");

	fprintf(file, "[FTTexture Path]\n");
	// This ensures the Animation with animName has been created.
	//std::wstring strPath = GetTexture()->GetRelativePath();
	//strName = std::string(strPath.begin(), strPath.end());
	fprintf(file, strName.c_str());
	fprintf(file, "\n");

	fclose(file);
}

//void AnimatorComponent::LoadAnimation(const std::wstring& path)
//{
//	FILE* file = nullptr;
//	_wfopen_s(&file, path.c_str(), L"rb");
//	assert(file);
//
//	char szBuff[256] = {};
//	LoadDataFromFile(szBuff, file);
//	LoadDataFromFile(szBuff, file);
//
//	std::string str;
//	str = szBuff;
//	std::wstring animationName = std::wstring(str.begin(), str.end());
//	if (FindAnimation(animationName) != nullptr)
//	{
//		SDL_Log("Animation with name %ls already exists. Aborting Animation loading...");
//		return;
//	}
//	else
//	{
//		LoadDataFromFile(szBuff, file);
//		LoadDataFromFile(szBuff, file);
//		std::string strFrame;
//		strFrame = szBuff;
//		size_t frameCount = std::stoi(std::wstring(strFrame.begin(), strFrame.end()));
//
//		Animation* animation = new Animation;
//		animation->SetAnimator(this);
//		animation->SetName(animationName);
//		animation->GetReel().resize(frameCount);
//
//		LoadDataFromFile(szBuff, file);
//
//		for (size_t i = 0; i < frameCount; ++i)
//		{
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			std::string strTileWidth;
//			strTileWidth = szBuff;
//
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			std::string strTileHeight;
//			strTileHeight = szBuff;
//
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			std::string strTileIndex;
//			strTileIndex = szBuff;
//
//			LoadDataFromFile(szBuff, file);
//			LoadDataFromFile(szBuff, file);
//			std::string strDuration;
//			strDuration = szBuff;
//
//			animation->GetReel()[i].tile.SetTileWidth(std::stoi(strTileWidth));
//			animation->GetReel()[i].tile.SetTileHeight(std::stoi(strTileHeight));
//			animation->GetReel()[i].tile.SetTileIndex(std::stoi(strTileIndex));
//			animation->GetReel()[i].duration = std::stoi(strDuration);
//		}
//		mMapAnimation.insert(std::make_pair(animationName, animation));
//
//		LoadDataFromFile(szBuff, file);
//		LoadDataFromFile(szBuff, file);
//		std::string texKeyStr;
//		texKeyStr = szBuff;
//		std::wstring texKey = std::wstring(texKeyStr.begin(), texKeyStr.end());
//		
//		LoadDataFromFile(szBuff, file);
//		LoadDataFromFile(szBuff, file);
//		std::string texPathStr;
//		texPathStr = szBuff;
//		std::wstring texPath = std::wstring(texPathStr.begin(), texPathStr.end());
//		ResourceManager::GetInstance()->LoadTexture(texKey, texPath);
//		SetTexture(ResourceManager::GetInstance()->GetLoadedTexture(texKey));
//	}
//	fclose(file);
//}

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
			FTVector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
			// DrawIndividualTileOnPosEX(renderer, worldPos, mCurrentAnim->GetTile());
		}
	}
}

void AnimatorComponent::EditorLateUpdate(float deltaTime)
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