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
	SpriteRendererComponent::Update(deltaTime);
	if (0 < mMaxFrame) {
		if (mIsPlaying) {
			mCurrFrame += mAnimFPS * deltaTime;
			while (mCurrFrame >= mMaxFrame)
				if (mIsRepeated)
					mCurrFrame -= mMaxFrame;
				else {
					mIsPlaying = false;
					mCurrFrame = 0;
					break;
				}
		}
	}
}

void SpriteAnimComponent::Render(FoxtrotRenderer* renderer){
	if(FrameIsWithinIndexRange())
		GetMeshGroup()->Render(renderer->GetContext(), static_cast<int>(mCurrFrame));
}

void SpriteAnimComponent::Play()
{
	mIsPlaying = true;
}

void SpriteAnimComponent::Stop()
{
	mIsPlaying = false;
}

bool SpriteAnimComponent::FrameIsWithinIndexRange()
{
	return 0 <= mCurrFrame && mCurrFrame < mMaxFrame;
}

#ifdef FOXTROT_EDITOR
void SpriteAnimComponent::EditorUIUpdate()
{
	this->UpdateSprite();
	this->UpdateCSV();
	this->UpdateIsRepeated();
	this->UpdatePlayAnim();
	this->OnConfirmUpdate();
}

void SpriteAnimComponent::UpdateIsRepeated()
{
	ImGui::Checkbox("Is Repeated", &mIsRepeated);
}

void SpriteAnimComponent::UpdatePlayAnim()
{
	if (mIsPlaying)
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
			Play();
		}
	}
}

void SpriteAnimComponent::OnConfirmUpdate()
{
	/*if (ImGui::Button("Update")) {
		if (!GetCSVFileMapPath().empty())
			this->InitializeAnimation();
		else
			printf("ERROR: SpriteAnimComponent::ConfirmUpdate() -> .CSV path is null");
	}*/
}

void SpriteAnimComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["MeshKey"], GetMeshKey());
	FileIOHelper::AddScalarValue(out["TextureKey"], GetTexKey());
	FileIOHelper::AddScalarValue(out["AnimationFPS"], mAnimFPS);
	FileIOHelper::AddScalarValue(out["IsRepeated"], mIsRepeated);
}
#endif