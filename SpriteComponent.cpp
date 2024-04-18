#include "SpriteComponent.h"
#include "Actor.h"
#include "Scene.h"
#include "CCore.h"
#include "FTTexture.h"
#include "Camera2D.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "ResourceManager.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	:Component(owner, drawOrder)
	, ptTexture(nullptr)
	, mTextWidth(0)
	, mTexHeight(0)
	, rect{}
{}

SpriteComponent::~SpriteComponent()
{
	delete rect;
}

void SpriteComponent::Update(float deltaTime)
{
	Vector2 screenPos = GetOwner()->GetTransform()->GetScreenPosition();
	if (rect != nullptr)
	{
		rect->w = static_cast<int>(mTextWidth * GetOwner()->GetTransform()->GetScale().x);
		rect->h = static_cast<int>(mTexHeight * GetOwner()->GetTransform()->GetScale().y);

		rect->x = static_cast<int>(screenPos.x + rect->w * 0.5f);
		rect->y = static_cast<int>(screenPos.y + rect->h * 0.5f);
	}
}

#ifdef _DEBUG
void SpriteComponent::Render(FoxtrotRenderer* renderer)
{
	if (ptTexture)
	{
		/*
		
		TEXTURE
		RENDER CODE HERE
		(Alternative to SDL_RenderCopy())
		
		*/
		BlitToGameview(rect, GetOwner()->GetTransform()->GetScale());
	}
}
void SpriteComponent::SetTexture(FTTexture* texture)
{
	ptTexture = texture;
	/*

		TEXTURE
		Get CODE HERE
		(Alternative to SDL_QueryTexture())

	*/
	rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
	rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
	rect->w = mTextWidth;
	rect->h = mTexHeight;
}

void SpriteComponent::EditorUpdate(float deltaTime)
{
	Vector2 screenPos = GetOwner()->GetTransform()->GetScreenPosition();
	if (rect != nullptr)
	{
		rect->w = static_cast<int>(mTextWidth * GetOwner()->GetTransform()->GetScale().x);
		rect->h = static_cast<int>(mTexHeight * GetOwner()->GetTransform()->GetScale().y);

		rect->x = static_cast<int>(screenPos.x + rect->w * 0.5f);
		rect->y = static_cast<int>(screenPos.y + rect->h * 0.5f);
	}
}

void SpriteComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::AddWString(ofs, ptTexture->GetKey());
	FileIOHelper::AddWString(ofs, ptTexture->GetRelativePath());
	FileIOHelper::AddInt(ofs, mTextWidth);
	FileIOHelper::AddInt(ofs, mTexHeight);
}

void SpriteComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	FileIOHelper::LoadInt(ifs, mTextWidth);
	FileIOHelper::LoadInt(ifs, mTexHeight);

	std::wstring textureKey = FileIOHelper::LoadWString(ifs);
	SetTexture(ResourceManager::GetInstance()->GetLoadedTexture(textureKey));
	std::wstring texturePath = FileIOHelper::LoadWString(ifs);
	ptTexture->SetRelativePath(texturePath);
}

void SpriteComponent::BlitToGameview(Bounds* blitArea, Vector2 scale)
{
	//FoxtrotRenderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	Vector2 renderCenter = Vector2(renderArea->x, renderArea->y);
	Vector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	Vector2 relativePos = worldPos - renderCenter;

	Bounds gameViewRect =
	{
		relativePos.x - GetOwner()->GetTransform()->GetScale().x * 0.5f,
		relativePos.y - GetOwner()->GetTransform()->GetScale().y * 0.5f,
		scale.x,
		scale.y
	};

	/*

		TEXTURE
		RENDER CODE HERE
		(Alternative to SDL_RenderCopy())

	*/
}

void SpriteComponent::BlitToGameviewEx(Bounds* blitArea, Vector2 scale)
{
	//FoxtrotRenderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	Vector2 renderCenter = Vector2(renderArea->x, renderArea->y);
	Vector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	Vector2 relativePos = worldPos - renderCenter;

	Bounds gameViewRect =
	{
		relativePos.x - GetOwner()->GetTransform()->GetScale().x * 0.5f,
		relativePos.y - GetOwner()->GetTransform()->GetScale().y * 0.5f,
		scale.x,
		scale.y
	};

	/*

		TEXTURE
		RENDER CODE HERE
		(Alternative to SDL_RenderCopy())

	*/
}
#else
void SpriteComponent::Render(FoxtrotRenderer* renderer)
{
	if (ptTexture)
	{
		SDL_RenderCopyEx(renderer,
			GetTexture()->GetGameviewTexture(),
			nullptr,
			rect,
			-Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
			nullptr,
			SDL_FLIP_NONE
		);
	}
}

void SpriteComponent::SetTexture(FTTexture* texture)
{
	ptTexture = texture;
	SDL_QueryTexture(ptTexture->GetGameviewTexture(), nullptr, nullptr, &mTextWidth, &mTexHeight);
	rect = new SDL_Rect;
	rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
	rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
	rect->w = mTextWidth;
	rect->h = mTexHeight;
}
#endif