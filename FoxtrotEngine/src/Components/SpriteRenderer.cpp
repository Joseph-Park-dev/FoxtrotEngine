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
	#include <EditorCamera.h>

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
	SetRenderer(coreInstance->GetGameRenderer());
	SetMeshKey(ChunkKey::PRIMITIVE_SQUARE_BLUE);
	MeshRenderer::Initialize(coreInstance);
}

void SpriteRenderer::Render(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		this->UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		GetMeshGroup()->Render(renderer);
	}
}

void SpriteRenderer::CloneTo(Actor* actor)
{
	SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
	newComp->SetMeshKey(GetMeshKey());
	newComp->SetTexKey(GetTexKey());

	for (size_t i = 0; i < MaterialKeys().size(); ++i)
		newComp->MaterialKeys().push_back(MaterialKeys().at(i));

	// newComp->GetMeshGroup()->SetDrawNormal(this->GetMeshGroup()->GetDrawNormal());
	newComp->mChannel  = this->mChannel;
	newComp->mTexScale = this->mTexScale;
}

SpriteRenderer::SpriteRenderer(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mChannel(4)
	, mTexScale(FTVector2(1.0f, 1.0f))
{
}

void SpriteRenderer::UpdateMesh(Transform* transform, Camera* camInst, FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		GetMeshGroup()->CalcVCData(transform, camInst);
		if (GetTexture())
		{
			FTVector2 texSize = GetTexture()->GetTexSize();
			Vector3	  scale	  = Vector3(texSize.x / texSize.y, 1.0f, 1.0f);
			GetMeshGroup()->GetVCData().model *= Matrix::CreateScale(scale);
		}
		GetMeshGroup()->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
	}
}

#ifdef FOXTROT_EDITOR
void SpriteRenderer::EditorUpdate(float deltaTime)
{
}

void SpriteRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	if (GetMeshGroup())
	{
		this->UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		GetMeshGroup()->SetTexture();
		GetMeshGroup()->Render(renderer);
	}
}

void SpriteRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());

	if (!GetMeshGroup())
		return;

	UpdateMaterial();
	OnConfirmUpdate();
	UpdateSprite();
	OnResetTexture();

	UpdateVS();
	UpdatePS();
}
#endif // FOXTROT_EDITOR