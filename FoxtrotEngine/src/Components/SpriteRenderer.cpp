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
#include "ResourceSystem/FTMeshGroup.h"
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
	#include <imgui.h>
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>
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
	MeshRenderer::Initialize(coreInstance);
	FTDS::String	  key	 = ChunkKey::PRIMITIVE_SQUARE_BLUE;
	FTMeshGroup* square = ResourceManager::GetInstance()->GetLoadedMesh(key);
	SetMeshGroup(square);
}

void SpriteRenderer::CloneTo(Actor* actor)
{
	SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
	newComp->SetRenderer(this->GetRenderer());
	newComp->SetMeshGroup(this->GetMeshGroup());
	newComp->SetTexture(this->GetTexture());
	newComp->SetVS(this->GetVS());
	newComp->SetPS(this->GetPS());
	newComp->SetMaterial(this->GetMaterial());

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
			Vector3	  size	  = Vector3(texSize.x / texSize.y, 1.0f, 1.0f);
			Vector3	  scale	  = Vector3(mTexScale.x, mTexScale.y, 1.0f);
			GetMeshGroup()->GetVCData().model *= 
				Matrix::CreateScale(size) * Matrix::CreateScale(scale);
		}
		GetMeshGroup()->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext(), GetMaterial());
	}
}

void SpriteRenderer::SaveProperties(std::ofstream& ofs)
{
	MeshRenderer::SaveProperties(ofs);
	FileIOHelper::SaveVector2(ofs, ChunkKey::SPRITE_SCALE, mTexScale);
}

void SpriteRenderer::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadVector2(ifs, mTexScale);
	MeshRenderer::LoadProperties(ifs);
}

void SpriteRenderer::EditorUIUpdate()
{
	MeshRenderer::EditorUIUpdate();
	CommandHistory::GetInstance()->UpdateVector2Value("TexScale", mTexScale);
}