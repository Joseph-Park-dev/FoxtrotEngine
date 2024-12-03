#include "Components/SpriteAnimComponent.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"

SpriteAnimComponent::SpriteAnimComponent(Actor* owner, int drawOrder, int updateOrder)
	: TileMapComponent(owner, drawOrder)
	, mAnimFPS(24.f)
	, mIsRepeated(true)
	, mCurrFrame(0.f)
	, mMaxFrame(0)
	, mIsPlaying(true)
{}

void SpriteAnimComponent::InitializeAnimation()
{
	//SetTileSizeOnMap(FTVector2(320, 320));
	//SetTileSizeOnScreen(FTVector2(1, 1));
	////SetTexKey("SpriteAnim");
	//if (!GetCSVFileMapPath().empty()) {
	//	SetTexture();
	//	assert(GetTexture());
	//	SetMaxCountOnMapX(GetTexture()->GetTexWidth() / GetTileSizeOnMap().x);
	//	SetMaxCountOnMapY(GetTexture()->GetTexHeight() / GetTileSizeOnMap().y);
	//	ReadCSV();
	//	assert(GetMaxCountOnScreenX() != 0 && GetMaxCountOnScreenY() != 0);
	//	ResourceManager::GetInstance()->LoadBasicMesh(
	//		GetCSVFileMapPath(),
	//		GeometryGenerator::MakeSpriteAnimation(
	//			GetCurrentTileMap(),
	//			GetMaxCountOnScreenX(),
	//			GetMaxCountOnScreenY()
	//		)
	//	);
	//	SetMeshKey(GetCSVFileMapPath());
	//	MeshRendererComponent::InitializeMesh();
	//	mMaxFrame = GetMeshGroup()->GetMeshCount();

	//	GetOwner()->GetTransform()->SetScale(FTVector3(0.3f, 0.3f, 0.0f));
	//}
}

void SpriteAnimComponent::Initialize(FTCore* coreInstance){
	MeshRendererComponent::Initialize(coreInstance);
	InitializeAnimation();
}

void SpriteAnimComponent::Update(float deltaTime) {
	
}

void SpriteAnimComponent::Render(FoxtrotRenderer* renderer){
}

void SpriteAnimComponent::Play()
{
	mIsPlaying = true;
}

void SpriteAnimComponent::Stop()
{
	mIsPlaying = false;
}


#ifdef FOXTROT_EDITOR
#endif