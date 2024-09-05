#include "FoxtrotEngine/Components/SpriteAnimComponent.h"

#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"

SpriteAnimComponent::SpriteAnimComponent(Actor* owner, int drawOrder, int updateOrder)
	: TileMapComponent(owner, drawOrder)
	, mCurrFrame(0.f)
	, mAnimFPS(24.f)
{
}

void SpriteAnimComponent::InitializeAnimation()
{
	SetTileSizeOnMap(FTVector2(64, 29));
	SetTileSizeOnScreen(FTVector2(1, 1));
	SetTexKey("SpriteAnim");
}

void SpriteAnimComponent::Initialize(FTCore* coreInstance){
	MeshRendererComponent::Initialize(coreInstance);
	InitializeAnimation();
	SetCSVTileMapPath("./assets/ShipTileMap.csv");
	if (!GetCSVFilePath().empty()) {
		SetTexture();
		assert(GetTexture());
		SetMaxCountOnMapX(GetTexture()->GetTexWidth() / GetTileSizeOnMap().x);
		SetMaxCountOnMapY(GetTexture()->GetTexHeight() / GetTileSizeOnMap().y);
		ReadCSV();
		assert(GetMaxCountOnScreenX() != 0 && GetMaxCountOnScreenY() != 0);
		ResourceManager::GetInstance()->LoadBasicMesh(
			GetCSVFilePath(),
			GeometryGenerator::MakeSpriteAnimation(
				GetCurrentTileMap(),
				GetMaxCountOnScreenX(),
				GetMaxCountOnScreenY()
			)
		);
		SetMeshKey(GetCSVFilePath());
		MeshRendererComponent::InitializeMesh();
		mMaxFrame = GetMeshGroup()->GetMeshCount();

		GetOwner()->GetTransform()->SetScale(FTVector3(0.3f, 0.18f, 0.0f));
	}
}

void SpriteAnimComponent::Update(float deltaTime) {
	SpriteRendererComponent::Update(deltaTime);
	mCurrFrame += mAnimFPS * deltaTime;
	while (mCurrFrame >= mMaxFrame)
	{
		mCurrFrame -= mMaxFrame;
	}
}

void SpriteAnimComponent::Render(FoxtrotRenderer* renderer){
	GetMeshGroup()->Render(renderer->GetContext(), static_cast<int>(mCurrFrame));
}

//void SpriteAnimComponent::SetAnimTextures(const std::vector<FTTexture*>& textures)
//{
//	mAnimTextures = textures;
//	if (mAnimTextures.size() > 0)
//	{
//		mCurrFrame = 0.0f;
//		//SetTexture(mAnimTextures[0]);
//	}
//}
