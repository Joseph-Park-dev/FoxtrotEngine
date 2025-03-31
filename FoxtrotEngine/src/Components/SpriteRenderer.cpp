// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteRenderer.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Math/FTMath.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMeshData.h"
#include "Scenes/Scene.h"

#ifdef FOXTROT_EDITOR
#include "CommandHistory.h"
#include "EditorUtils.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>
#endif // FOXTROT_EDITOR

int SpriteRenderer::GetTexWidth()
{
	return GetTexture()->GetTexWidth() * static_cast<int>(mTexScale.x);
}

int SpriteRenderer::GetTexHeight()
{
	return GetTexture()->GetTexHeight() * static_cast<int>(mTexScale.y);
}

void SpriteRenderer::Initialize(FTCore* coreInstance)
{
	SetRenderer	(coreInstance->GetGameRenderer());

	if(GetOwner()->GetActorGroup() == ActorGroup::PLAYER)
		SetMeshKey	(ChunkKey::PRIMITIVE_SQUARE_BLUE);
	else if (GetOwner()->GetActorGroup() == ActorGroup::ENEMY)
		SetMeshKey(ChunkKey::PRIMITIVE_SQUARE_RED);
	else
		SetMeshKey(ChunkKey::PRIMITIVE_SQUARE_GREEN);

	MeshRenderer::Initialize(coreInstance);
}

void SpriteRenderer::CloneTo(Actor* actor)
{
	SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
	newComp->SetMeshKey(GetMeshKey());
	newComp->SetTexKey(GetTexKey());

	for (size_t i = 0; i < MaterialKeys().size(); ++i)
		newComp->MaterialKeys().push_back(MaterialKeys().at(i));

	newComp->GetMeshGroup()->SetDrawNormal(this->GetMeshGroup()->GetDrawNormal());
	newComp->mChannel = this->mChannel;
	newComp->mTexScale = this->mTexScale;
}

bool SpriteRenderer::InitializeMesh()
{
	MeshRenderer::InitializeMesh(
		ResourceManager::GetInstance()->GetLoaded2DPrimitive(
			GetMeshKey()
		)
	);
	if (!GetMeshGroup())
	{
		LogString("ERROR: SpriteRenderer::InitializeMesh() -> Mesh "
			"Init failed");
		return false;
	}
	return true;
}

SpriteRenderer::SpriteRenderer(Actor* owner,int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mChannel(4)
	, mTexScale(FTVector2(1.0f,1.0f)) 
{}

#ifdef FOXTROT_EDITOR
void SpriteRenderer::EditorUpdate(float deltaTime)
{
}

void SpriteRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	UpdateMaterial();
	OnConfirmUpdate();
	UpdateSprite();
	OnResetTexture();
}
#endif // FOXTROT_EDITOR