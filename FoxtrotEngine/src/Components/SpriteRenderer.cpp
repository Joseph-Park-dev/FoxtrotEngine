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

void SpriteRenderer::Initialize(FTCore* coreInstance)
{
	MeshRenderer::Initialize(coreInstance);
	FTDS::String key	= ChunkKey::PRIMITIVE_SQUARE_SPRITE;
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
}

SpriteRenderer::SpriteRenderer(Actor* owner, int updateOrder)
	: MeshRenderer(owner, updateOrder)
	, mChannel(4)
{
}

void SpriteRenderer::SaveProperties(std::ofstream& ofs)
{
	MeshRenderer::SaveProperties(ofs);
}

void SpriteRenderer::LoadProperties(std::ifstream& ifs)
{
	MeshRenderer::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void SpriteRenderer::EditorUIUpdate()
{
	MeshRenderer::EditorUIUpdate();
}
#endif // FOXTROT_EDITOR